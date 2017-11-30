//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SHADER_HPP
#define XR_SHADER_HPP

#include "ShaderComponent.hpp"
#include "Asset.hpp"
#include "Gfx.hpp"

namespace XR
{

//==============================================================================
///@brief Provides a shader pipeline complete with vertex and fragment shaders.
class Shader: public Asset
{
public:
  XR_ASSET_DECL(Shader)

  // general
  Gfx::ProgramHandle GetHandle() const // no ownership transfer
  {
    return m_handle;
  }

  bool OnLoaded(size_t size, uint8_t const * buffer) override;
  void OnUnload() override;

protected:
  // data
  ShaderComponent::Ptr  m_vertexShader;
  ShaderComponent::Ptr  m_fragmentShader;

  Gfx::ProgramHandle m_handle;
};

}

#endif //XR_SHADER_HPP