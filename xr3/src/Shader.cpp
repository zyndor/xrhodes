//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Shader.hpp"
#include "XR/XonBuildTree.hpp"
#include "XR/BufferReader.hpp"
#include "XR/streamutils.hpp"

namespace XR
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
  auto root = XonBuildTree(buffer.As<char const>(), buffer.size);
  bool success = root != nullptr;
  if (success)
  {
    try
    {
      // Read path to vertex shader; add to dependencies and write hash.
      auto vertexEntry = root->Find("vsh").GetValue();
      FilePath  path(vertexEntry);
      if (path.GetExt() == nullptr)
      {
        path += ".vsh";
      }
      dependencies.push_back(path);

      Asset::HashType hash = Asset::Manager::HashPath(path);
      success = WriteBinaryStream(hash, data);

      if (success)
      {
        // Read path to fragment shader; add to dependencies and write hash.
        auto fragmentEntry = root->Find("fsh").GetValue();
        path = fragmentEntry;
        if (path.GetExt() == nullptr)
        {
          path += ".fsh";
        }
        dependencies.push_back(path);

        hash = Asset::Manager::HashPath(path);
        success = WriteBinaryStream(hash, data);
      }
    }
    catch (XonEntity::Exception const&)
    {
      success = false;
    }
  }

  return success;
}
#endif

}

//==============================================================================
bool Shader::SetComponents(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment)
{
  bool success = vertex->GetType() == Gfx::ShaderType::Vertex &&
    fragment->GetType() == Gfx::ShaderType::Fragment;
  if (!success)
  {
    XR_TRACE(Shader, ("Invalid input to '%s' (check components).", m_debugPath.c_str()));
  }

  if (success)
  {
    Gfx::Destroy(m_handle);
    m_handle = Gfx::CreateProgram(vertex->GetHandle(), fragment->GetHandle());

    success = m_handle.IsValid();
    if (!success)
    {
      XR_TRACE(Shader, ("Failed to create program for '%s'.", m_debugPath.c_str()));
    }
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
  auto flags = GetFlags();
  BufferReader reader(buffer);

  HashType hash;
  bool success = reader.Read(hash);
  if (!success)
  {
    XR_TRACE(Shader, ("Failed to read vertex shader hash for '%s'.", m_debugPath.c_str()));
  }

  ShaderComponent::Ptr cVertex;
  if (success)  // Read vertex shader
  {
    cVertex = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    success = cVertex.Get();
    if (!success)
    {
      XR_TRACE(Shader, ("Failed to load fragment shader component for '%s'.", m_debugPath.c_str()));
    }
  }

  if (success)
  {
    success = reader.Read(hash);
    if (!success)
    {
      XR_TRACE(Shader, ("Failed to read fragment shader hash for '%s'.", m_debugPath.c_str()));
    }
  }

  ShaderComponent::Ptr cFragment;
  if (success)  // read fragment shader
  {
    cFragment = Manager::Load(Descriptor<ShaderComponent>(hash), flags);  // we'll check for failure later.
    if (!success)
    {
      XR_TRACE(Shader, ("Failed to load fragment shader component for '%s'.", m_debugPath.c_str()));
    }
  }

  if (success)
  {
    success = SetComponents(cVertex, cFragment);
  }

  return success;
}

//==============================================================================
void Shader::OnUnload()
{
  Gfx::Destroy(m_handle);
  m_handle.Invalidate();

  m_vertexShader.Reset(nullptr);
  m_fragmentShader.Reset(nullptr);
}

}
