//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Material.hpp"
#include "xr/FileWriter.hpp"
#include "xr/XonBuildTree.hpp"
#include "xr/BufferReader.hpp"
#include "xr/streamutils.hpp"
#include "xr/stringutils.hpp"
#include <algorithm>
#include <unordered_map>

#define LTRACE(format) XR_TRACE(Material, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Material, condition, format)

namespace xr
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
const std::string kBlendFactorKeys[4] = {
  "srcColor",
  "dstColor",
  "srcAlpha",
  "dstAlpha",
};

static class MaterialBuilder : public Asset::Builder
{
public:
  /*
    {
      state: { depthWrite, depthTest, alphaBlend },
      blendFactors: { srcAlpha, invSrcAlpha },
      depthFunc: lequal,
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
    m_states{
      { Hash::String32("depthTest"), Gfx::F_STATE_DEPTH_TEST },
      { Hash::String32("depthWrite"), Gfx::F_STATE_DEPTH_WRITE },
      { Hash::String32("alphaBlend"), Gfx::F_STATE_ALPHA_BLEND },
      { Hash::String32("cullBack"), Gfx::F_STATE_CULL_BACK },
      { Hash::String32("cullFront"), Gfx::F_STATE_CULL_FRONT },
      { Hash::String32("wireframe"), Gfx::F_STATE_WIREFRAME }, // experimental
    },
    m_comparisonFunctions{
      { Hash::String32("fail"), Gfx::Comparison::FAIL },
      { Hash::String32("less"), Gfx::Comparison::LESS },
      { Hash::String32("lequal"), Gfx::Comparison::LEQUAL },
      { Hash::String32("equal"), Gfx::Comparison::EQUAL },
      { Hash::String32("gequal"), Gfx::Comparison::GEQUAL },
      { Hash::String32("greater"), Gfx::Comparison::GREATER },
      { Hash::String32("nequal"), Gfx::Comparison::NEQUAL },
      { Hash::String32("pass"), Gfx::Comparison::PASS },
    },
    m_blendFactors{
      { Hash::String32("zero"), Gfx::BlendFactor::ZERO },
      { Hash::String32("one"), Gfx::BlendFactor::ONE },
      { Hash::String32("srcColor"), Gfx::BlendFactor::SRC_COLOR },
      { Hash::String32("invSrcColor"), Gfx::BlendFactor::INV_SRC_COLOR },
      { Hash::String32("srcAlpha"), Gfx::BlendFactor::SRC_ALPHA },
      { Hash::String32("invSrcAlpha"), Gfx::BlendFactor::INV_SRC_ALPHA },
      { Hash::String32("dstColor"), Gfx::BlendFactor::DST_COLOR },
      { Hash::String32("invDstColor"), Gfx::BlendFactor::INV_DST_COLOR },
      { Hash::String32("dstAlpha"), Gfx::BlendFactor::DST_ALPHA },
      { Hash::String32("invDstAlpha"), Gfx::BlendFactor::INV_DST_ALPHA },
    }
  {}

  bool Build(char const* rawNameExt, Buffer buffer,
    std::vector<FilePath>& dependencies, std::ostream& data) const override
  {
    XonParser::State state;
    std::unique_ptr<XonObject> root(XonBuildTree(buffer.As<char const>(), buffer.size, &state));
    bool success = root != nullptr;
    LTRACEIF(!success,
      ("%s: failed to parse XON somewhere around row %d, column %d.", rawNameExt,
        state.row, state.column));
    if (success)  // process state flags
    {
      Gfx::FlagType stateFlags = Gfx::F_STATE_NONE;
      if(auto state = root->TryGet("state"))
      {
        switch (state->GetType())
        {
        case XonEntity::Type::Value:
          {
            auto stateValue = state->ToValue().GetString();
            if (!InterpretState(stateValue, stateFlags))
            {
              LTRACE(("%s: Unsupported state: %s", rawNameExt, stateValue));
            }
          }
          break;

        case XonEntity::Type::Object:
          {
            auto& stateObject = state->ToObject();
            for (size_t i = 0; i < stateObject.GetNumElements(); ++i)
            {
              auto& elem = stateObject[i];
              if (elem.GetType() == XonEntity::Type::Value &&
                !InterpretState(elem.ToValue().GetString(), stateFlags))
              {
                LTRACE(("%s: Unsupported state: %s.", rawNameExt, elem.ToValue().GetString()));
              }
            }
          }
          break;
        }
      }

      if (CheckAllMaskBits(stateFlags, Gfx::F_STATE_DEPTH_TEST))
      {
        // depth compare function -- doesn't have to exist, but if it does, it
        // needs to be the correct format
        try {
          auto& xon = root->Get("depthFunc").ToValue();
          Gfx::Comparison::Value depthFunc;
          success = InterpretComparison(xon.GetString(), depthFunc);
          if (success)
          {
            stateFlags |= Gfx::DepthFuncToState(depthFunc);
          }
          else
          {
            LTRACE(("%s: Unsupported depth function: %s.", rawNameExt,
             xon.GetString()));
          }
        }
        catch (XonEntity::Exception const& e)
        {
          if (e.type == XonEntity::Exception::Type::InvalidType)
          {
            success = false;
            LTRACE(("%s: invalid depth function definition, error: %s", rawNameExt, e.what()));
          }
        }
      }
      else if (root->TryGet("depthFunc"))
      {
        LTRACE(("%s: ignoring depth function defined when depth testing is off.",
          rawNameExt));
      }

      if (success && CheckAllMaskBits(stateFlags, Gfx::F_STATE_ALPHA_BLEND))
      {
        // blendFactors -- doesn't have to exist, but if it does, it has to be an
        // object with specific value elements.
        try {
          auto& blendFactors = root->Get("blendFactors").ToObject();
          XonEntity* xon[4] = {
            blendFactors.TryGet(kBlendFactorKeys[0]),
            blendFactors.TryGet(kBlendFactorKeys[1]),
            blendFactors.TryGet(kBlendFactorKeys[2]),
            blendFactors.TryGet(kBlendFactorKeys[3]),
          };

          Gfx::BlendFactor::Value values[4];
          // Interpret color blend factors. These must exist.
          for (int i = 0; success && i < 2; ++i)
          {
            success = xon[i] && InterpretBlendFactor(xon[i]->ToValue().GetString(), values[i]);
            if (success)
            {
              values[i + 2] = values[i];  // default alpha blend factors to corresponding rgb blend factors.
            }
            else
            {
              LTRACE(("%s: failed to interpret %s blend factor.", rawNameExt, kBlendFactorKeys[i].c_str()));
            }
          }

          // Interpret alpha blend factors. These are optional, but must be valid if specified.
          for (int i = 2; success && i < 4; ++i)
          {
            if (xon[i])
            {
              if (auto stringValue = xon[i]->ToValue().GetString())
              {
                success = InterpretBlendFactor(stringValue, values[i]);
                LTRACEIF(!success, ("%s: failed to interpret %s blend factor.", rawNameExt, kBlendFactorKeys[i].c_str()));
              }
            }
          }

          if (success)
          {
            stateFlags |= Gfx::BlendFactorToState(values[0], values[2], values[1], values[3]);
          }
        }
        catch (XonEntity::Exception const& e)
        {
          if (e.type == XonEntity::Exception::Type::InvalidType)
          {
            success = false;
            LTRACE(("%s: invalid blend factors definition, error: %s", rawNameExt, e.what()));
          }
        }
      }
      else if(root->TryGet("blendFactors"))
      {
        LTRACE(("%s: ignoring blend factors defined for opaque material.",
          rawNameExt));
      }

      if (success)
      {
        success = WriteBinaryStream(stateFlags, data);
        LTRACEIF(!success, ("%s: failed to write state.", rawNameExt));
      }
    }

    if (success)  // process texture stages
    {
      std::vector<SerializedTextureStage>  stages;

      try {
        auto& textures = root->Get("textures");

        FilePath path;
        if (textures.GetType() == XonEntity::Type::Value)
        {
          // single texture for stage 0
          path = textures.ToValue().GetString();
          dependencies.push_back(path);

          Asset::HashType hash = Asset::Manager::HashPath(path);
          stages.push_back({ 0, hash });
        }
        else // object
        {
          auto& texturesObject = textures.ToObject();
          stages.reserve(std::min(texturesObject.GetNumElements(),
            static_cast<size_t>(Material::kMaxTextureStages)));

          std::vector<std::string> kBlendFactorKeys;
          static_cast<XonObject&>(textures).GetKeys(kBlendFactorKeys);
          for (auto &key: kBlendFactorKeys)
          {
            uint32_t stage; // avoid interpretation as character.
            if (StringTo(key.c_str(), stage) &&
              stage < Material::kMaxTextureStages)
            {
              try {
                auto& texture = texturesObject.Get(key);
                if(texture.GetType() == XonEntity::Type::Value)
                {
                  path = texture.ToValue().GetString();
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
        auto& shader = root->Get("shader").ToValue();

        FilePath path = shader.GetString();
        dependencies.push_back(path);

        auto hash = Asset::Manager::HashPath(path);
        success = WriteBinaryStream(hash, data);
        LTRACEIF(!success, ("%s: failed to write shader hash."));
      }
      catch (XonEntity::Exception const&)
      {
        success = false;
        LTRACE(("%s: missing or invalid %s definition.", rawNameExt, "shader"));
      }
    }
    return success;
  }

protected:
  std::unordered_map<uint32_t, uint32_t> m_states;
  std::unordered_map<uint32_t, Gfx::Comparison::Value> m_comparisonFunctions;
  std::unordered_map<uint32_t, Gfx::BlendFactor::Value> m_blendFactors;

  bool InterpretState(char const* value, Gfx::FlagType& flags) const
  {
    auto iFind = m_states.find(Hash::String32(value));
    const bool success = iFind != m_states.end();
    if (success)
    {
      flags |= iFind->second;
    }
    return success;
  }

  bool InterpretComparison(char const* value, Gfx::Comparison::Value& comparison) const
  {
    auto iFind = m_comparisonFunctions.find(Hash::String32(value));
    const bool success = iFind != m_comparisonFunctions.end();
    if (success)
    {
      comparison = iFind->second;
    }
    return success;
  }

  bool InterpretBlendFactor(char const* value, Gfx::BlendFactor::Value& blendFactor) const
  {
    auto iFind = m_blendFactors.find(Hash::String32(value));
    const bool success = iFind != m_blendFactors.end();
    if (success)
    {
      blendFactor = iFind->second;
    }
    return success;
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
void Material::OverrideStateFlags(Gfx::FlagType clear, Gfx::FlagType set)
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

  m_shader->Use();
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
    auto ptr = Manager::Load(static_cast<Descriptor<Texture>&>(desc), flags);
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
    auto ptr = Manager::Load(static_cast<Descriptor<Shader>&>(desc), flags);
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
  m_stateFlags = Gfx::F_STATE_NONE;
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
Shader::Ptr Material::GetShader() const
{
  return m_shader;
}

} // XR
