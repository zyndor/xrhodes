#include  "xrgl.hpp"
#include  "Material.hpp"
#include  "MaterialImpl.hpp"

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
  colAmbient(1.0f, 1.0f, 1.0f, 1.0f)
{}

//==============================================================================
MaterialImpl::~MaterialImpl()
{}

//==============================================================================
void  MaterialImpl::Apply()
{
  XR_ASSERT(MaterialImpl, arpTextures[0] != 0);
  XR_GL_CALL(glColor4f(colAmbient.r, colAmbient.g, colAmbient.b, colAmbient.a));
  XR_GL_CALL(glEnable(GL_DEPTH_TEST));
  if(arpTextures[0] == 0)
  {
    XR_GL_CALL(glDisable(GL_TEXTURE_2D));
  }
  else
  {
    XR_GL_CALL(glEnable(GL_TEXTURE_2D));
    XR_GL_CALL(glActiveTexture(GL_TEXTURE0));
    arpTextures[0]->Bind(GL_TEXTURE_2D);

    if(alphaMode == Material::ALPHA_NONE)
    {
      XR_GL_CALL(glDisable(GL_BLEND));
    }
    else
    {
      XR_GL_CALL(glEnable(GL_BLEND));
      switch(alphaMode)
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

    if(alphaTestMode == GL_NEVER - 1)
    {
      XR_GL_CALL(glDisable(GL_ALPHA_TEST));
    }
    else
    {
      XR_GL_CALL(glEnable(GL_ALPHA_TEST));
      XR_GL_CALL(glAlphaFunc(alphaTestMode, alphaTestRefValue));
    }

    if(cullMode != GL_NONE)
    {
      XR_GL_CALL(glEnable(GL_CULL_FACE));
      XR_GL_CALL(glCullFace(cullMode));
    }
    else
    {
      XR_GL_CALL(glDisable(GL_CULL_FACE));
    }

    XR_GL_CALL(glDepthMask(depthWriteEnabled));

    if(arpTextures[1] != 0)
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