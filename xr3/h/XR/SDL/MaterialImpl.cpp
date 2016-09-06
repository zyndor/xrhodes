//
// Nuclear Heart Interactive
// XRhodes
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#include "xrgl.hpp"
#include "MaterialImpl.hpp"
#include <XR/Material.hpp>

namespace XR
{

//==============================================================================
MaterialImpl::MaterialImpl()
: arpTextures(),
  alphaMode(Material::ALPHA_NONE),
  alphaTestMode(GL_GREATER),
  alphaTestRefValue(.0f),
  blendMode(Material::BLEND_ADD),
  cullMode(GL_BACK),
  depthWriteEnabled(GL_TRUE),
  colAmbient(1.0f, 1.0f, 1.0f, 1.0f),
  colDiffuse(1.0f, 1.0f, 1.0f, 1.0f),
  colSpecular(.0f, .0f, .0f, .0f),
  colEmissive(.0f, .0f, .0f, .0f)
{}

//==============================================================================
MaterialImpl::~MaterialImpl()
{}

//==============================================================================
void  MaterialImpl::Apply()
{
  XR_GL_CALL(glColor4f(colAmbient.r, colAmbient.g, colAmbient.b, colAmbient.a));
  XR_GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colDiffuse.arData));
  XR_GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colSpecular.arData));
  XR_GL_CALL(glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colEmissive.arData));
  XR_GL_CALL(glEnable(GL_DEPTH_TEST));

  if (alphaMode == Material::ALPHA_NONE)
  {
    XR_GL_CALL(glDisable(GL_BLEND));
  }
  else
  {
    XR_GL_CALL(glEnable(GL_BLEND));
    switch (alphaMode)
    {
    case  Material::ALPHA_ADD:
      XR_GL_CALL(glBlendFunc(GL_ONE, GL_ONE));
      break;

    case  Material::ALPHA_SUB:
      XR_GL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE));
      break;

    case  Material::ALPHA_BLEND:
    case  Material::ALPHA_DEFAULT:
    case  Material::ALPHA_HALF: // stub
      XR_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      break;
    }
  }

  if (alphaTestMode == GL_NEVER - 1)
  {
    XR_GL_CALL(glDisable(GL_ALPHA_TEST));
  }
  else
  {
    XR_GL_CALL(glEnable(GL_ALPHA_TEST));
    XR_GL_CALL(glAlphaFunc(alphaTestMode, alphaTestRefValue));
  }

  if (cullMode != GL_NONE)
  {
    XR_GL_CALL(glEnable(GL_CULL_FACE));
    XR_GL_CALL(glCullFace(cullMode));
  }
  else
  {
    XR_GL_CALL(glDisable(GL_CULL_FACE));
  }

  XR_GL_CALL(glDepthMask(depthWriteEnabled));

  if (arpTextures[0] == 0)
  {
    XR_GL_CALL(glDisable(GL_TEXTURE_2D));
  }
  else
  {
    XR_GL_CALL(glEnable(GL_TEXTURE_2D));
    XR_GL_CALL(glActiveTexture(GL_TEXTURE0));
    arpTextures[0]->Bind(GL_TEXTURE_2D);

    if (arpTextures[1] != 0)
    {
      // todo: secondary texture
      XR_GL_CALL(glActiveTexture(GL_TEXTURE1));
      arpTextures[1]->Bind(GL_TEXTURE_2D);

      // todo: blend mode  
    }
  }
}

//==============================================================================
void  MaterialImpl::Copy(const MaterialImpl& rhs)
{
  memcpy(this, &rhs, sizeof(MaterialImpl));
}

} // XR