#ifndef XR_SHADER_HPP
#define XR_SHADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "ShaderComponent.hpp"
#include "Asset.hpp"
#include "Gfx.hpp"

namespace xr
{

//==============================================================================
///@brief Provides a shader pipeline, complete with vertex and fragment shaders,
/// creatable from a .shd file, which is a XON document with support for the
/// following properties:
/// vsh: path to the vertex shader definition (*.vsh may be omitted); required.
/// fsh: path to the fragment shader definition (*.fsh may be omitted); required.
///@note Asset options may be supplied (when loading directly or referencing from
/// another asset) to be forwarded to all shader components e.g.
/// terrain.shd$DEFERRED$ALPHA_TEST (the meaning of these defines dopends on your
/// shader implementation). Please refer to the documentation of/ ShaderComponent
/// for further details.
class Shader: public Asset
{
public:
  XR_ASSET_DECL(Shader)

  // general
  ///@return Whether the shader is successfully created.
  bool IsValid() const
  {
    return m_handle.IsValid();
  }

  ///@brief Sets the shader as current, for use.
  void Use() const
  {
    Gfx::SetProgram(m_handle);
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

  bool SetComponentsInternal(ShaderComponent::Ptr vertex, ShaderComponent::Ptr fragment);
};

}

#endif //XR_SHADER_HPP
