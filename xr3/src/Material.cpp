//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Material.hpp"
#include "XR/FileWriter.hpp"
#include "XR/XonBuildTree.hpp"
#include "XR/BufferReader.hpp"
#include "XR/streamutils.hpp"
#include "XR/stringutils.hpp"
#include <algorithm>
#include <unordered_map>

namespace XR
{

//==============================================================================
XR_ASSET_DEF(Material, "xmtl", 1, "mtl")

//==============================================================================
namespace
{

#ifdef ENABLE_ASSET_BUILDING
static class MaterialBuilder : public Asset::Builder
{
public:
  /*
    {
      state: { depthWrite, depthTest, alphaBlend },
      textures: {
        0: "textures/shark/albedo.png",
        3: "textures/shark/roughness.png"
      },
      tints: {
        0: { .5, 1.0, 1.0, .5 }
      }
    }
  */
  MaterialBuilder()
  : Builder(Material::kTypeId),
    m_states()
  {
    m_states[Hash::String32("depthTest")] = Gfx::F_STATE_DEPTH_TEST;
    m_states[Hash::String32("depthWrite")] = Gfx::F_STATE_DEPTH_WRITE;
    m_states[Hash::String32("alphaBlend")] = Gfx::F_STATE_ALPHA_BLEND;
    m_states[Hash::String32("cullBack")] = Gfx::F_STATE_CULL_BACK;
    m_states[Hash::String32("cullFront")] = Gfx::F_STATE_CULL_FRONT;

    // experimental
    m_states[Hash::String32("wireframe")] = Gfx::F_STATE_WIREFRAME;
  }

  bool Build(char const* rawNameExt, Buffer buffer,
    std::vector<FilePath>& dependencies, std::ostream& data) const override
  {
    XonObject* root = XonBuildTree(buffer.As<char const>(), buffer.size);
    bool success = root != nullptr;
    if (success)  // process stage flags
    {
      try {
        uint32_t flags = 0;
        auto& state = root->Find("state");
        switch (state.GetType())
        {
        case XonEntity::Type::Value:
          InterpretState(state.GetValue(), flags);
          break;

        case XonEntity::Type::Object:
          for (auto i = 0; i < state.GetNumElements(); ++i)
          {
            if (state[i].GetType() == XonEntity::Type::Value)
            {
              InterpretState(state[i].GetValue(), flags);
            }
          }
          break;
        }

        success = WriteBinaryStream(flags, data);
      }
      catch (XonEntity::Exception const&)
      {
        success = false;
      }
    }

    if (success)  // process texture stages
    {
      struct TextureData
      {
        uint8_t stage;
        Asset::HashType hash;
      };

      std::vector<TextureData>  stages;

      try {
        auto& textures = root->Find("textures");

        FilePath path;
        if (textures.GetType() == XonEntity::Type::Value)
        {
          // single texture for stage 0
          path = textures.GetValue();
          dependencies.push_back(path);

          Asset::HashType hash = Asset::Manager::HashPath(path);
          stages.push_back({ 0, hash });
        }
        else
        {
          stages.reserve(std::min(textures.GetNumElements(),
            static_cast<size_t>(Material::kMaxTextureStages)));

          std::vector<std::string> keys;
          static_cast<XonObject&>(textures).GetKeys(keys);
          uint16_t stage;  // Stage is totally a uint8_t, but we don't want StringTo()'s stream extraction to interpret it as a character; we'll cast it back upon writing, promise.
          for (size_t i = 0; success && i < stages.capacity(); ++i)
          {
            if (StringTo(keys[i].c_str(), stage) && stage < Material::kMaxTextureStages)
            {
              try {
                auto& texture = textures.Find(keys[i]);
                if (texture.GetType() == XonEntity::Type::Value)
                {
                  path = texture.GetValue();
                  dependencies.push_back(path);

                  Asset::HashType hash = Asset::Manager::HashPath(path);
                  stages.push_back({ uint8_t(stage), hash });
                }
              }
              catch (XonEntity::Exception const&) // the texture that we were told we're going to have isn't there.
              {
                success = false;
                break;
              }
            }
            else
            {
              success = false;
            }
          }
        }
      }
      catch (XonEntity::Exception const&) // couldn't find textures - not an error
      {}

      if (success)  // write out texture stages
      {
        success = WriteBinaryStream(uint8_t(stages.size()), data);

        for (auto& stage : stages)
        {
          success = success && WriteBinaryStream(stage.stage, data) &&
            WriteBinaryStream(stage.hash, data);
        }
      }
    }

    // TODO: tints?

    if (success)
    {
      // shader
      try {
        auto& shader = root->Find("shader");

        FilePath path = shader.GetValue();
        dependencies.push_back(path);

        auto hash = Asset::Manager::HashPath(path);
        success = WriteBinaryStream(hash, data);
      }
      catch (XonEntity::Exception const&)
      {
        success = false;
      }
    }
    return success;
  }

  std::unordered_map<uint32_t, uint32_t> m_states;

  void InterpretState(char const* value, uint32_t& flags) const
  {
    auto iFind = m_states.find(Hash::String32(value));
    if (iFind != m_states.end())
    {
      flags |= iFind->second;
    }
    else
    {
      XR_TRACE(MaterialBuilder, ("Unsupported state: %s", value));
    }
  }
} materialBuilder;
#endif

}

//==============================================================================
uint32_t Material::GetStateFlags() const
{
  return m_stateFlags;
}

//==============================================================================
void Material::OverrideStateFlags(uint32_t clear, uint32_t set)
{
  m_stateFlags = (m_stateFlags & ~clear) | set;
}

//==============================================================================
Texture::Ptr Material::GetTexture(int stage) const
{
  return m_textureStages[stage];
}

//==============================================================================
void  Material::SetTexture(int stage, Texture::Ptr const& texture)
{
  m_textureStages[stage] = texture;
}

//==============================================================================
void Material::Apply() const
{
  Gfx::SetState(m_stateFlags);
  for (int i = 0; i < kMaxTextureStages; ++i)
  {
    auto const& texture = m_textureStages[i];
    if (texture)
    {
      texture->Bind(i);
    }
    else
    {
      Gfx::SetTexture(Gfx::GetDefaultTexture2D(), i);
    }
  }

  Gfx::SetProgram(m_shader->GetHandle());
}

//==============================================================================
bool Material::OnLoaded(Buffer buffer)
{
  auto flags = GetFlags();

  BufferReader  reader(buffer);
  uint8_t numTextures;
  bool success = reader.Read(m_stateFlags) && reader.Read(numTextures);

  uint8_t stage;
  DescriptorCore desc(Texture::kTypeId);
  for (auto i = 0; success && i < numTextures; ++i) // read texture stages and hashes
  {
    success = reader.Read(stage) && reader.Read(desc.hash);
    if (success)  // get texture
    {
      auto ptr = Asset::Manager::Load(static_cast<Descriptor<Texture>&>(desc), flags); // TODO: remove Asset::
      m_textureStages[stage] = ptr;
      success = ptr.Get() != nullptr;
    }
  }

  if (success)  // read shader hash
  {
    desc.type = Shader::kTypeId;
    success = reader.Read(desc.hash);
  }

  if (success)  // get shader
  {
    auto ptr = Asset::Manager::Load(static_cast<Descriptor<Shader>&>(desc), flags); // TODO: remove Asset::
    m_shader = ptr;
    success = ptr.Get() != nullptr;
  }

  // TODO tints

  return success;
}

//==============================================================================
void Material::OnUnload()
{
  m_stateFlags = 0;
  for (auto i = 0; i < kMaxTextureStages; ++i)
  {
    m_textureStages[i].Reset(nullptr);
  }

  m_shader.Reset(nullptr);
}

//==============================================================================
void Material::SetShader(Shader::Ptr const & shader)
{
  m_shader = shader;
}

//==============================================================================
Shader::Ptr Material::GetShader()
{
  return m_shader;
}

} // XR
