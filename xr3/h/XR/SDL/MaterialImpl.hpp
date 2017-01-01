//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MATERIALIMPL_HPP
#define XR_MATERIALIMPL_HPP

#include "TextureImpl.hpp"
#include <XR/Color.hpp>
#include <XR/utils.hpp>

namespace XR
{

//==============================================================================
class MaterialImpl
{
public:
  // types
  enum
  {
    kNumTextureStages = 2
  };

  // data
  TextureImpl*  arpTextures[kNumTextureStages];

  uint32        alphaMode;
  GLenum        alphaTestMode;
  GLfloat       alphaTestRefValue;
  uint32        blendMode;
  GLenum        cullMode;
  GLboolean     depthWriteEnabled;
  Color         colAmbient;
  Color         colDiffuse;
  Color         colSpecular;
  Color         colEmissive;

  // structors
  MaterialImpl();
  ~MaterialImpl();
  
  // general
  TextureImpl*  GetTexture(int32 id) const;

  void  Apply();
  void  Copy(const MaterialImpl& rhs);
};

//==============================================================================
//  implementation
//==============================================================================
inline 
TextureImpl*  MaterialImpl::GetTexture(int32 id) const
{
  XR_ASSERT(Material, id >= 0);
  XR_ASSERT(Material, id < MaterialImpl::kNumTextureStages);
  return arpTextures[id];
}

} // XR

#endif