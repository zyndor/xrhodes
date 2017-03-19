//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include  "debugdraw.hpp"

namespace XR
{

namespace DebugDraw
{

//==============================================================================
static uint32_t   s_lastFlush(0);
static Material*  s_pMaterial(0);

static void SetMaterial()
{
  uint32_t  flushId(Renderer::GetFlushId());
  if(s_lastFlush < flushId || flushId + INT32_MAX > s_lastFlush + INT32_MAX)
  {
    s_pMaterial = Renderer::AllocMaterial(); 
    s_lastFlush = flushId;
  }

  Renderer::SetMaterial(s_pMaterial);
}

//==============================================================================
void  Line(const Vector3& v, Material* pMaterial)
{
  FloatBuffer*  pStream(Renderer::AllocBuffer(sizeof(Vector3), 2));
  pStream->Set(0, Vector3::Zero());
  pStream->Set(1, v);

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_LINE_STRIP);
}

//==============================================================================
void  LineStrip(const Vector3* parVerts, int numVerts, Material* pMaterial)
{
  FloatBuffer*  pStream(Renderer::AllocBuffer(sizeof(Vector3), numVerts));
  for(int i = 0; i < numVerts; ++i)
  {
    pStream->Set(i, *parVerts);
    ++parVerts;
  }

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_LINE_STRIP);
}

//==============================================================================
void  LineList(const Vector3* parVerts, int numVerts, Material* pMaterial)
{
  FloatBuffer*  pStream(Renderer::AllocBuffer(sizeof(Vector3), numVerts));
  for(int i = 0; i < numVerts; ++i)
  {
    pStream->Set(i, *parVerts);
    ++parVerts;
  }

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_LINE_LIST);
}

//==============================================================================
void  Rect(float hw, float hh, Material* pMaterial)
{
  FloatBuffer* pStream(Renderer::AllocBuffer(sizeof(Vector3), 5));
  pStream->Set(0, Vector3(-hw, -hh, .0f));
  pStream->Set(1, Vector3(-hw, hh, .0f));
  pStream->Set(2, Vector3(hw, hh, .0f));
  pStream->Set(3, Vector3(hw, -hh, .0f));
  pStream->Set(4, pStream->Get<Vector3>(0));

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_LINE_STRIP);
}

//==============================================================================
void  FillRect(float hw, float hh, Material* pMaterial)
{
  FloatBuffer* pStream(Renderer::AllocBuffer(sizeof(Vector3), 4));
  pStream->Set(0, Vector3(-hw, -hh, .0f));
  pStream->Set(1, Vector3(-hw, hh, .0f));
  pStream->Set(2, Vector3(hw, hh, .0f));
  pStream->Set(3, Vector3(hw, -hh, .0f));

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_TRI_FAN);
}

//==============================================================================
void  Circle(float radius, Material* pMaterial)
{
  int numVerts(floorf(radius * .666f));
  float theta(M_PI / numVerts);
  float c(cosf(theta));
  float s(sinf(theta));
  Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  ++numVerts;
  FloatBuffer* pStream(Renderer::AllocBuffer(sizeof(Vector3), numVerts));
  for(int i = 0; i < numVerts; ++i)
  {
    pStream->Set(i, v);
    v = Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
  }
  
  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_LINE_STRIP);
}

//==============================================================================
void  FillCircle(float radius, Material* pMaterial)
{
  int numVerts(floorf(radius * .666f));
  float theta(M_PI / numVerts);
  float c(cosf(theta));
  float s(sinf(theta));
  Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  numVerts += 2;
  FloatBuffer* pStream(Renderer::AllocBuffer(sizeof(Vector3), numVerts));
  pStream->Set(0, Vector3::Zero());
  for(int i = 1; i < numVerts; ++i)
  {
    pStream->Set(i, v);
    v = Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
  }

  if(pMaterial == 0)
  {
    SetMaterial();
  }
  else
  {
    Renderer::SetMaterial(pMaterial);
  }
  Renderer::SetVertStream(*pStream);
  Renderer::SetColStream();
  Renderer::DrawPrims(PRIM_TRI_FAN);
}

} // DebugDraw

} // XR
