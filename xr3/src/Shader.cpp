//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Shader.hpp"
#include "xr/memory/BufferReader.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "ParseAssetOptions.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include "xr/io/streamutils.hpp"
#include <set>
#endif

#define LTRACE(format) XR_TRACE(Shader, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Shader, "xshd", 2, "shd")

namespace {

XR_ASSET_BUILDER_DECL(Shader)

#ifdef ENABLE_ASSET_BUILDING
XR_ASSET_BUILDER_BUILD_SIG(Shader)
{
  /*
    {
      vsh: "path/to/vertex_shader", # vsh extension is optional
      fsh: "path/to/vertex_shader", # fsh extension is optional
    }
  */
  // Get a string with all the options.
  auto options = [rawNameExt]() {
    std::set<std::string> lOptions;
    ParseAssetOptions(rawNameExt, [rawNameExt, &lOptions](char const* option) {
      if (!lOptions.insert(option).second)
      {
        LTRACE(("%s: ignored repeated instance of option '%s'.", rawNameExt, option));
      }
      return true;
    });

    std::ostringstream strOptions;
    for (auto& o : lOptions)
    {
      strOptions << Asset::kOptionDelimiter << o;
    }

    return strOptions.str();
  }();

  XonParser::State state;
  std::unique_ptr<XonObject>  root(XonBuildTree(buffer.As<char const>(), buffer.size, &state));
  if (!root)
  {
    LTRACE(("%s: failed to parse XON somewhere around row %d, column %d.",
      rawNameExt, state.row, state.column));
    return false;
  }

  try
  {
    // Read path to vertex shader; add to dependencies and write hash.
    FilePath vertexEntry = root->Get("vsh").ToValue().GetString();
    if (vertexEntry.GetExt() == nullptr)
    {
      vertexEntry += ".vsh";
    }
    vertexEntry += options.c_str();
    dependencies.push_back(vertexEntry);

    Asset::HashType hash = Asset::Manager::HashPath(vertexEntry);
    if (!WriteBinaryStream(hash, data))
    {
      LTRACE(("%s: failed to write vertex shader hash.", rawNameExt));
      return false;
    }
  }
  catch (XonEntity::Exception const&)
  {
    LTRACE(("%s: missing vertex shader definition.", rawNameExt));
    return false;
  }

  try
  {
    // Read path to fragment shader; add to dependencies and write hash.
    FilePath fragmentEntry = root->Get("fsh").ToValue().GetString();
    if (fragmentEntry.GetExt() == nullptr)
    {
      fragmentEntry += ".fsh";
    }
    fragmentEntry += options.c_str();
    dependencies.push_back(fragmentEntry);

    Asset::HashType hash = Asset::Manager::HashPath(fragmentEntry);
    if (!WriteBinaryStream(hash, data))
    {
      LTRACE(("%s: failed to write fragment shader hash.", rawNameExt));
      return false;
    }
  }
  catch (XonEntity::Exception const&)
  {
    LTRACE(("%s: missing fragment shader definition.", rawNameExt));
    return false;
  }

  return true;
}
#endif

}

//==============================================================================
bool Shader::SetComponents(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment)
{
  XR_DEBUG_ONLY(auto flags = GetFlags());
  XR_ASSERT(ShaderComponent, CheckAllMaskBits(flags, ErrorFlag) ||
    !CheckAnyMaskBits(flags, LoadingFlag | ProcessingFlag));

  bool success = SetComponentsInternal(vertex, fragment);
  OverrideFlags(PrivateMask, success ? ReadyFlag : (ProcessingFlag | ErrorFlag));

  return success;
}

//==============================================================================
bool Shader::OnLoaded(Buffer buffer)
{
  const auto flags = GetFlags();
  BufferReader reader(buffer);

  HashType hash;
  if (!reader.Read(hash))
  {
    LTRACE(("%s: failed to read %s shader hash.", m_debugPath.c_str(), "vertex"));
    return false;
  }

  auto cVertex = Manager::Load(Descriptor<ShaderComponent>(hash), flags);
  if (!cVertex)
  {
    LTRACE(("%s: failed to load %s shader component.", m_debugPath.c_str(), "vertex"));
    return false;
  }

  if (!reader.Read(hash))
  {
    LTRACE(("%s: failed to read %s shader hash.", m_debugPath.c_str(), "fragment"));
    return false;
  }

  auto cFragment = Manager::Load(Descriptor<ShaderComponent>(hash), flags);
  if (!cFragment)
  {
    LTRACE(("%s: failed to load %s shader component.", m_debugPath.c_str(), "fragment"));
    return false;
  }

#ifdef ENABLE_ASSET_BUILDING
  if (CheckAllMaskBits(flags, DryRunFlag))
  {
    return true;
  }
#endif
  return SetComponentsInternal(cVertex, cFragment);
}

//==============================================================================
void Shader::OnUnload()
{
  Gfx::Release(m_handle);
  m_handle.Invalidate();

  m_vertexShader.Reset(nullptr);
  m_fragmentShader.Reset(nullptr);
}

//==============================================================================
bool Shader::SetComponentsInternal(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment)
{
  auto hVertex = vertex->GetHandle();
  auto hFragment = fragment->GetHandle();
  if (!hVertex.IsValid())
  {
    LTRACE(("%s: %s shader component is invalid.", m_debugPath.c_str(), "vertex"));
    return false;
  }

  if (vertex->GetType() != Gfx::ShaderType::Vertex)
  {
    LTRACE(("%s: %s input has invalid type.", m_debugPath.c_str(), "vertex"));
    return false;
  }

  if (!hFragment.IsValid())
  {
    LTRACE(("%s: %s shader component is invalid.", m_debugPath.c_str(), "fragment"));
    return false;
  }

  if (fragment->GetType() != Gfx::ShaderType::Fragment)
  {
    LTRACE(("%s: %s input has invalid type.", m_debugPath.c_str(), "fragment"));
    return false;
  }

  Gfx::Release(m_handle);
  m_handle = Gfx::CreateProgram(hVertex, hFragment);

  if (!m_handle.IsValid())
  {
    LTRACE(("%s: failed to create shader program.", m_debugPath.c_str()));
    return false;
  }

  m_vertexShader = vertex;
  m_fragmentShader = fragment;

  return true;
}

}
