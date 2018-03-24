#ifndef XR_SHADER_HPP
#define XR_SHADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "ShaderComponent.hpp"
#include "Asset.hpp"
#include "Gfx.hpp"

namespace XR
{

//==============================================================================
///@brief Provides a shader pipeline complete with vertex and fragment shaders,
/// creatable from a .shd file, which is a XON document with support for the
/// following properties:
/// vsh: path to the vertex shader definition; required.
/// fsh: path to the fragment shader definition; required.
class Shader: public Asset
{
public:
  XR_ASSET_DECL(Shader)

  // general
  ///@return The handle to the underlying Gfx shader program.
  ///@note Does not transfer ownership; intended as read only.
  Gfx::ProgramHandle GetHandle() const
  {
    return m_handle;
  }

  ///@brief Sets the shader components, (re-)links the underlying program.
  bool SetComponents(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment);

protected:
  // data
  ShaderComponent::Ptr  m_vertexShader;
  ShaderComponent::Ptr  m_fragmentShader;

  Gfx::ProgramHandle m_handle;

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

}

#endif //XR_SHADER_HPP
