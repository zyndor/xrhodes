//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/IMaterialisable.hpp"

namespace xr
{

//==============================================================================
Material::Ptr IMaterialisable::Materialise(Material::Ptr const & material)
{
  return Materialise(material->GetStateFlags(), material->GetShader());
}

//==============================================================================
Material::Ptr IMaterialisable::Materialise(Gfx::FlagType state, Shader::Ptr const & shader)
{
  Material::Ptr material(Material::Create(0, Asset::UnmanagedFlag));
  material->OverrideStateFlags(0, state);
  material->SetShader(shader);
  material->SetTexture(0, GetTexture());
  return material;
}

}
