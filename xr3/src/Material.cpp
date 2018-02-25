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

#define LTRACE(format) XR_TRACE(Material, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Material, condition, format)

namespace XR
{

//==============================================================================
XR_ASSET_DEF(Material, "xmtl", 1, "mtl")

//==============================================================================
namespace
{

struct SerializedTextureStage
{
  uint8_t stage;
  Asset::HashType hash;
};

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
    XonParser::State state;
    XonObject* root = XonBuildTree(buffer.As<char const>(), buffer.size, &state);
    bool success = root != nullptr;
    LTRACEIF(!success,
      ("%s: failed to parse XON somewhere around row %d, column %d.", rawNameExt,
        state.row, state.column));
    if (success)  // process stage flags
    {
      uint32_t flags = 0;
      try {
        auto& state = root->Find("state");
        switch (state.GetType())
        {
        case XonEntity::Type::Value:
          InterpretState(state.GetValue(), flags);
          break;

        case XonEntity::Type::Object:
          for (size_t i = 0; i < state.GetNumElements(); ++i)
          {
            auto& elem = state[i];
            if (elem.GetType() == XonEntity::Type::Value)
            {
              InterpretState(elem.GetValue(), flags);
            }
          }
          break;
        }
      }
      catch (XonEntity::Exception const&)
      {}  // ignore missing state.

      success = WriteBinaryStream(flags, data);
      LTRACEIF(!success, ("%s: failed to write state.", rawNameExt));
    }

    if (success)  // process texture stages
    {
      std::vector<SerializedTextureStage>  stages;

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
        else // object
        {
          stages.reserve(std::min(textures.GetNumElements(),
            static_cast<size_t>(Material::kMaxTextureStages)));

          std::vector<std::string> keys;
          static_cast<XonObject&>(textures).GetKeys(keys);
          for (auto &key: keys)
          {
            uint32_t stage; // avoid interpretation as character.
            if (StringTo(key.c_str(), stage) &&
              stage < Material::kMaxTextureStages)
            {
              try {
                auto& texture = textures.Find(key);
                if(texture.GetType() == XonEntity::Type::Value)
                {
                  path = texture.GetValue();
                  dependencies.push_back(path);

                  Asset::HashType hash = Asset::Manager::HashPath(path);
                  stages.push_back({ uint8_t(stage), hash });
                }
                else
                {
                  LTRACE(("%s: ignoring texture %d: path not a value.",
                    rawNameExt, stage));
                }
              }
              catch(XonEntity::Exception const&)
              {
                LTRACE(("FATAL: %s: key '%s' declared but not found.",
                  rawNameExt, key.c_str()));
                abort();
              }
            }
            else
            {
              LTRACE(("%s: ignoring texture stage '%s' as invalid.", rawNameExt,
                key.c_str()));
            }
          }
        }
      }
      catch (XonEntity::Exception const&)
      {} // ignore missing textures.

      if (success)  // write out texture stages
      {
        success = WriteRangeBinaryStream<uint8_t>(stages.begin(), stages.end(),
          data);
        LTRACEIF(!success, ("%s: failed to write texture stages.", rawNameExt));
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
        LTRACEIF(!success, ("%s: failed to write shader hash."));
      }
      catch (XonEntity::Exception const&)
      {
        success = false;
        LTRACE(("%s: missing %s definition.", rawNameExt, "shader"));
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
      LTRACE(("Unsupported state: %s", value));
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
  bool success = reader.Read(m_stateFlags);
  LTRACEIF(!success, ("%s: failed to read state flags.", m_debugPath.c_str()));

  uint8_t numTextures = 0;
  SerializedTextureStage const* textures = nullptr;
  if (success)
  {
    textures = reader.ReadElementsWithSize<SerializedTextureStage>(numTextures);
    success = textures != nullptr;
    LTRACEIF(!success, ("%s: failed to read texture stages.", m_debugPath.c_str()));
  }

  DescriptorCore desc(Texture::kTypeId);
  for (decltype(numTextures) i = 0; success && i < numTextures; ++i)
  {
    desc.hash = textures[i].hash;
    auto ptr = Asset::Manager::Load(static_cast<Descriptor<Texture>&>(desc),
      flags); // TODO: remove Asset::
    success = ptr.Get() != nullptr;
    if (success)
    {
      m_textureStages[textures[i].stage] = ptr;
    }
    else
    {
      LTRACE(("%s: failed to retrieve texture %llx.", m_debugPath.c_str()));
    }
  }

  if (success)  // read shader hash
  {
    desc.type = Shader::kTypeId;
    success = reader.Read(desc.hash);
    LTRACEIF(!success, ("%s: failed to read shader hash.", m_debugPath.c_str()));
  }

  if (success)  // get shader
  {
    auto ptr = Asset::Manager::Load(static_cast<Descriptor<Shader>&>(desc), flags); // TODO: remove Asset::
    m_shader = ptr;
    success = ptr.Get() != nullptr;
    LTRACEIF(!success, ("%s: failed to retrieve shader %llx.", m_debugPath.c_str(),
      desc.hash));
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
