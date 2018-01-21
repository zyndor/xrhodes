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
  bool success = vertex.Get() && vertex->GetType() == Gfx::ShaderType::Vertex &&
    fragment.Get() && fragment->GetType() == Gfx::ShaderType::Fragment;

  if (success)
  {
    Gfx::Destroy(m_handle);
    m_handle = Gfx::CreateProgram(vertex->GetHandle(), fragment->GetHandle());

    success = m_handle.IsValid();
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
  ShaderComponent::Ptr cVertex;
  if (success)  // Read vertex shader
  {
    cVertex = Manager::Load(Descriptor<ShaderComponent>(hash), flags);
  }

  ShaderComponent::Ptr cFragment;
  if (success)  // read fragment shader
  {
    cFragment = Manager::Load(Descriptor<ShaderComponent>(hash), flags);
  }

  if (success)  // now link these babies
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
