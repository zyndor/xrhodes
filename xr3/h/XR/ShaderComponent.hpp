#ifndef XR_SHADERCOMPONENT_HPP
#define XR_SHADERCOMPONENT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Asset.hpp"
#include "Gfx.hpp"

namespace XR
{

//==============================================================================
///@brief Provides one part of the shader pipeline -- either a fragment or a
/// vertex shader.
class ShaderComponent: public Asset
{
public:
  XR_ASSET_DECL(ShaderComponent)

  // general
  Gfx::ShaderType GetType() const // TODO: we need an uninitialised value for our enum before we can pick up girls with this.
  {
    return m_type;
  }

  Gfx::ShaderHandle GetHandle() const // no ownership transfer
  {
    return m_handle;
  }

  bool SetSource(Gfx::ShaderType type, char const* source);

  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;

protected:
  // data
  Gfx::ShaderType m_type;
  Gfx::ShaderHandle m_handle;
  std::vector<uint8_t> m_data; // If KeepSourceDataFlag is set.
};

} // XR

#endif //XR_SHADER_HPP
