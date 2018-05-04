//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/IMaterialisable.hpp"

namespace XR
{

//==============================================================================
Material::Ptr IMaterialisable::Materialise(Material::Ptr const & material)
{
  return Materialise(material->GetStateFlags(), material->GetShader());
}

//==============================================================================
Material::Ptr IMaterialisable::Materialise(uint32_t state, Shader::Ptr const & shader)
{
  Material::Ptr material(Material::Create(0, Asset::UnmanagedFlag)->Cast<Material>());
  material->OverrideStateFlags(0, state);
  material->SetShader(shader);
  material->SetTexture(0, GetTexture());
  return material;
}

}
