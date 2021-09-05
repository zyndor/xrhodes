#ifndef XR_MATERIALISABLE_HPP
#define XR_MATERIALISABLE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace xr
{

//==============================================================================
///@brief Materialisable is an interface for classes that can provide a texture,
/// and require capability to create a Material, given a set of render states,
/// a Shader and the Texture.
class IMaterialisable
{
public:
  virtual Texture::Ptr GetTexture() const =0;

  ///@return A new Material which is based on the state flags and Shader that
  /// @a material has, but uses the texture which GetTexture() returns.
  ///@note This material is not managed by Asset::Manager.
  Material::Ptr Materialise(Material::Ptr const& material);

  ///@return A new Material which uses the given remder @a state, @a shader.
  /// and the Texture which GetTexture() returns.
  ///@note This material is not managed by Asset::Manager.
  Material::Ptr Materialise(Gfx::FlagType state, Shader::Ptr const& shader);

protected:
  virtual ~IMaterialisable() {}
};

}

#endif //XR_MATERIALISABLE_HPP
