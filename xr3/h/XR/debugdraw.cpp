//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  "debugdraw.hpp"

namespace XR
{

//==============================================================================
void  DebugDrawCircle(float radius)
{
  int numVerts(floorf(radius * .666f));
  float theta(M_PI / numVerts);
  float c(cosf(theta));
  float s(sinf(theta));
  XR::Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  ++numVerts;
  XR::RenderStream* pStream(XR::Renderer::AllocStream(XR::RenderStream::F_VECTOR3, numVerts));
  for(int i = 0; i < numVerts; ++i)
  {
    pStream->Set(i, v);
    v = XR::Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
  }

  XR::Material* pMat(XR::Renderer::AllocMaterial());
  XR::Renderer::SetMaterial(pMat);
  XR::Renderer::SetVertStream(*pStream);
  XR::Renderer::SetColStream();
  XR::Renderer::DrawPrims(XR::PRIM_LINE_STRIP);
}

//==============================================================================
void  DebugDrawFilledCircle(float radius)
{
  int numVerts(floorf(radius * .666f));
  float theta(M_PI / numVerts);
  float c(cosf(theta));
  float s(sinf(theta));
  XR::Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  numVerts += 2;
  XR::RenderStream* pStream(XR::Renderer::AllocStream(XR::RenderStream::F_VECTOR3, numVerts));
  pStream->Set(0, Vector3::s_zero);
  for(int i = 1; i < numVerts; ++i)
  {
    pStream->Set(i, v);
    v = XR::Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
  }

  XR::Material* pMat(XR::Renderer::AllocMaterial());
  XR::Renderer::SetMaterial(pMat);
  XR::Renderer::SetVertStream(*pStream);
  XR::Renderer::SetColStream();
  XR::Renderer::DrawPrims(XR::PRIM_TRI_FAN);
}

} // XR
