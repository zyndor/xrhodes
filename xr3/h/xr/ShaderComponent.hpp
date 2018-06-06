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

namespace xr
{

//==============================================================================
///@brief Provides one part of the shader pipeline -- either a fragment or a
/// vertex shader.
class ShaderComponent: public Asset
{
public:
  XR_ASSET_DECL(ShaderComponent)

  // general
  ///@return The type of the shader.
  ///@note Its results are only meaningful if GetHandle().IsValid().
  Gfx::ShaderType GetType() const
  {
    return m_type;
  }

  ///@return The handle to the underlying Gfx shader.
  ///@note Does not transfer ownership; intended as read only.
  Gfx::ShaderHandle GetHandle() const
  {
    return m_handle;
  }

  ///@brief (Re-)compiles the underlying shader from the given @a source.
  bool SetSource(Gfx::ShaderType type, char const* source);

protected:
  // data
  Gfx::ShaderType m_type;
  Gfx::ShaderHandle m_handle;
  std::vector<uint8_t> m_data; // If KeepSourceDataFlag is set.

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

} // XR

#endif //XR_SHADER_HPP
