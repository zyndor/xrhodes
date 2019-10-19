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
#define LTRACEIF(condition, format) XR_TRACEIF(Shader, condition, format)

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
    std::set<std::string> options;
    ParseAssetOptions(rawNameExt, [rawNameExt, &options](char const* option) {
      if (!options.insert(option).second)
      {
        LTRACE(("%s: ignored repeated instance of option '%s'.",
          rawNameExt, option));
      }
      return true;
    });

    std::ostringstream strOptions;
    for (auto& o : options)
    {
      strOptions << Asset::kOptionDelimiter << o;
    }

    return strOptions.str();
  }();

  XonParser::State state;
  std::unique_ptr<XonObject>  root(XonBuildTree(buffer.As<char const>(), buffer.size, &state));
  bool success = root != nullptr;
  LTRACEIF(!success,
    ("%s: failed to parse XON somewhere around row %d, column %d.",
      rawNameExt, state.row, state.column));

  if (success)
  {
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
      success = WriteBinaryStream(hash, data);
      LTRACEIF(!success, ("%s: failed to write vertex shader hash.",
        rawNameExt));
    }
    catch (XonEntity::Exception const&)
    {
      success = false;
      LTRACE(("%s: missing vertex shader definition.", rawNameExt));
    }
  }

  if (success)
  {
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
      success = WriteBinaryStream(hash, data);
      LTRACEIF(!success, ("%s: failed to write fragment shader hash.",
        rawNameExt));
    }
    catch (XonEntity::Exception const&)
    {
      success = false;
      LTRACE(("%s: missing fragment shader definition.", rawNameExt));
    }
  }

  return success;
}
#endif

}

//==============================================================================
bool Shader::SetComponents(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment)
{
  // NOTE: since this method is called from OnLoaded(), we can't include the
  // ProcessingFlag in the below check. Nor did I want to create a separate
  // SetComponentsInternal(), leaving only an assert here.
  XR_DEBUG_ONLY(auto flags = GetFlags());
  XR_ASSERT(ShaderComponent, CheckAllMaskBits(flags, ErrorFlag) ||
    !CheckAnyMaskBits(flags, LoadingFlag));

  auto hVertex = vertex->GetHandle();
  auto hFragment = fragment->GetHandle();
  bool success = hVertex.IsValid();
  LTRACEIF(!success, ("%s: %s shader component is invalid.", m_debugPath.c_str(),
    "vertex"));

  if (success)
  {
    success = vertex->GetType() == Gfx::ShaderType::Vertex;
    LTRACEIF(!success, ("%s: %s input has invalid type.", m_debugPath.c_str(), "vertex"));
  }

  if (success)
  {
    success = hFragment.IsValid();
    LTRACEIF(!success, ("%s: %s shader component is invalid.", m_debugPath.c_str(),
      "fragment"));
  }

  if (success)
  {
    success = fragment->GetType() == Gfx::ShaderType::Fragment;
    LTRACEIF(!success, ("%s: %s input has invalid type.", m_debugPath.c_str(), "fragment"));
  }

  if (success)
  {
    Gfx::Release(m_handle);
    m_handle = Gfx::CreateProgram(hVertex, hFragment);

    success = m_handle.IsValid();
    LTRACEIF(!success, ("%s: failed to create shader program.", m_debugPath.c_str()));
  }

  if (success)
  {
    m_vertexShader = vertex;
    m_fragmentShader = fragment;
  }

  OverrideFlags(PrivateMask, success ? ReadyFlag : (ProcessingFlag | ErrorFlag));

  return success;
}

//==============================================================================
bool Shader::OnLoaded(Buffer buffer)
{
  const auto flags = GetFlags();
  BufferReader reader(buffer);

  HashType hash;
  bool success = reader.Read(hash);
  LTRACEIF(!success, ("%s: failed to read %s shader hash.",
    m_debugPath.c_str(), "vertex"));

  ShaderComponent::Ptr cVertex;
  if (success)  // Read vertex shader
  {
    cVertex = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    success = cVertex != nullptr;
    LTRACEIF(!success, ("%s: failed to load %s shader component.",
      m_debugPath.c_str(), "vertex"));
  }

  if (success)
  {
    success = reader.Read(hash);
    LTRACEIF(!success, ("%s: failed to read %s shader hash.",
      m_debugPath.c_str(), "fragment"));
  }

  ShaderComponent::Ptr cFragment;
  if (success)  // read fragment shader
  {
    cFragment = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    success = cFragment != nullptr;
    LTRACEIF(!success, ("%s: failed to load %s shader component.", m_debugPath.c_str(),
      "fragment"));
  }

  if (
#ifdef ENABLE_ASSET_BUILDING
    !CheckAllMaskBits(flags, DryRunFlag) &&
#endif
    success)
  {
    success = SetComponents(cVertex, cFragment);
  }

  return success;
}

//==============================================================================
void Shader::OnUnload()
{
  Gfx::Release(m_handle);
  m_handle.Invalidate();

  m_vertexShader.Reset(nullptr);
  m_fragmentShader.Reset(nullptr);
}

}
