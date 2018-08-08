//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Shader.hpp"
#include "xr/XonBuildTree.hpp"
#include "xr/BufferReader.hpp"
#include "xr/streamutils.hpp"

#define LTRACE(format) XR_TRACE(Shader, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Shader, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Shader, "xshd", 1, "shd")

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
  auto hVertex = vertex->GetHandle();
  auto hFragment = fragment->GetHandle();
  bool success = hVertex.IsValid() && vertex->GetType() == Gfx::ShaderType::Vertex;
  LTRACEIF(!success, ("%s: vertex input has invalid type.", m_debugPath.c_str()));

  if (success)
  {
    success = hFragment.IsValid() && fragment->GetType() == Gfx::ShaderType::Fragment;
    LTRACEIF(!success, ("%s: fragment input has invalid type.",
      m_debugPath.c_str()));
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
  return success;
}

//==============================================================================
bool Shader::OnLoaded(Buffer buffer)
{
  const auto flags = GetFlags();
  BufferReader reader(buffer);

  HashType hash;
  bool success = reader.Read(hash);
  LTRACEIF(!success, ("%s: failed to read vertex shader hash.",
    m_debugPath.c_str()));

  ShaderComponent::Ptr cVertex;
  if (success)  // Read vertex shader
  {
    cVertex = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    success = cVertex != nullptr;
    LTRACEIF(!success, ("%s: failed to load vertex shader component.",
      m_debugPath.c_str()));
  }

  if (success)
  {
    success = reader.Read(hash);
    LTRACEIF(!success, ("%s: failed to read fragment shader hash.",
      m_debugPath.c_str()));
  }

  ShaderComponent::Ptr cFragment;
  if (success)  // read fragment shader
  {
    cFragment = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    success = cFragment != nullptr;
    LTRACEIF(!success,
      ("%s: failed to load fragment shader component.", m_debugPath.c_str()));
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
