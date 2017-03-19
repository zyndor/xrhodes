//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <IwGx.h>
#include "Renderer.hpp"
#include "ProjectionHelpers.hpp"

namespace XR
{

//==============================================================================
struct  RendererImpl
{
  // data
  float           zFar;
  float           zNear;
  float           tanHalfVerticalFov;
  Rect            scissorRect;
  int             numVertices;
  uint32          flushId;
};

static RendererImpl* s_pRenderer(0);

//==============================================================================
void Renderer::Init()
{
  XR_ASSERTMSG(Renderer, s_pRenderer == 0, ("Already initialised!"));

  s_pRenderer = new RendererImpl;

  s3eSurfaceSetInt(S3E_SURFACE_DEVICE_ORIENTATION_LOCK, S3E_SURFACE_LANDSCAPE);
  IwGxTickUpdate(); 
  IwGxFlush(); 
  IwGxSwapBuffers();
  s3eSurfaceSetInt(S3E_SURFACE_DEVICE_ORIENTATION_LOCK,
    S3E_SURFACE_ORIENTATION_FREE);

  IwGxSetSortMode(IW_GX_SORT_NONE);
  IwGxSetSortModeAlpha(IW_GX_SORT_NONE);
  IwGxSetSortModeOpaque(IW_GX_SORT_NONE);
  IwGxSetSortModeScreenSpace(IW_GX_SORT_NONE);
  
  s_pRenderer->zNear = IwGxGetNearZ();
  s_pRenderer->zFar = IwGxGetFarZ();
  
  CIwRect rect(IwGxGetScissorScreenSpace());
  s_pRenderer->scissorRect.x = rect.x;
  s_pRenderer->scissorRect.y = rect.y;
  s_pRenderer->scissorRect.w = rect.w;
  s_pRenderer->scissorRect.h = rect.h;
  s_pRenderer->numVertices = 0;

  SetPerspective(float(M_PI * .25), 10.0f, 1000.0f);

  s_pRenderer->flushId = 1;
}

//==============================================================================
void Renderer::Exit()
{
  XR_ASSERTMSG(Renderer, s_pRenderer != 0, ("Not initialised."));
  
  delete s_pRenderer;
  s_pRenderer = 0;
}

//==============================================================================
int32 Renderer::GetScreenWidth()
{
  return IwGxGetScreenWidth();
}

//==============================================================================
int32 Renderer::GetScreenHeight()
{
  return IwGxGetScreenHeight();
}

//==============================================================================
void* Renderer::Alloc(size_t bytes)
{
  XR_ASSERT(Renderer, bytes >= 0);
  return IwGxDataAlloc(bytes);
}

//==============================================================================
Material* Renderer::AllocMaterial()
{
  return new (XR_RENDERER_ALLOC(Material, 1)) Material(IW_GX_ALLOC_MATERIAL());
}

//==============================================================================
FloatBuffer* Renderer::AllocBuffer(size_t elemSize, size_t numElems)
{
  XR_ASSERT(Renderer, numElems >= 0);

  size_t bufferBytes(elemSize * numElems);
  void* pMem(Alloc(sizeof(FloatBuffer) + bufferBytes));
  FloatBuffer* pBuffer(new (pMem) FloatBuffer());
  pBuffer->SetBuffer(elemSize, numElems, reinterpret_cast<float*>(pBuffer + 1));
  return pStream;
}

//==============================================================================
void Renderer::SetPerspMatrix( const float (&arData)[kNumPersMatrixElems] )
{
  IwGxSetPerspectiveMatrix(const_cast<float*>(arData));
}

//==============================================================================
void Renderer::SetOrtho(float left, float right, float bottom, float top,
  float zNear, float zFar)
{
  XR_ASSERT(Renderer, left != right);
  XR_ASSERT(Renderer, top != bottom);
  XR_ASSERT(Renderer, zFar > .0f);
  //uint32  flags(IwGxGetFlags());
  //XR_TRACE(Renderer, ("ortho: %d", (flags & IW_GX_ORTHO_PROJ_F) != 0));
  //flags |= IW_GX_ORTHO_PROJ_F;
  //IwGxSetFlags(flags);
  
  if(zNear == .0f)
  {
    zNear = -zFar;
  }

  float arPerspMatrix[16];
  ProjectionHelpers::CalculateOrtho(left, right, bottom, top, zNear, zFar, arPerspMatrix);
  
  s_pRenderer->zNear = zNear;
  s_pRenderer->zFar = zFar;
  s_pRenderer->tanHalfVerticalFov = .0f;
  //IwGxSetFarZNearZ(zFar, zNear);
  IwGxSetPerspectiveMatrix(arPerspMatrix);
}

//==============================================================================
void Renderer::SetPerspective(float vFov, float aspect, float zNear,
  float zFar)
{
  //uint32  flags(IwGxGetFlags());
  //XR_TRACE(Renderer, ("ortho: %d", (flags & IW_GX_ORTHO_PROJ_F) != 0));
  //flags &= ~IW_GX_ORTHO_PROJ_F;
  //IwGxSetFlags(flags);

  float arPerspMatrix[16];
  ProjectionHelpers::CalculatePerspective(vFov, aspect, zNear, zFar, arPerspMatrix,
    &s_pRenderer->tanHalfVerticalFov);

  s_pRenderer->zNear = zNear;
  s_pRenderer->zFar = zFar;
  IwGxSetFarZNearZ(zFar, zNear);
  IwGxSetPerspectiveMatrix(arPerspMatrix);
}

//==============================================================================
void Renderer::SetPerspective(float verticalFov, float zNear, float zFar)
{
  SetPerspective(verticalFov, float(IwGxGetScreenWidth()) / float(IwGxGetScreenHeight()),
    zNear, zFar);
}

//==============================================================================
void Renderer::SetFarNearZ( float zFar, float zNear )
{
  XR_ASSERT(Renderer, zFar > zNear);
  s_pRenderer->zFar = zFar;
  s_pRenderer->zNear = zNear;
}

//==============================================================================
float Renderer::GetNearZ()
{
  return s_pRenderer->zNear;
}

//==============================================================================
float Renderer::GetFarZ()
{
  return s_pRenderer->zFar;
}

//==============================================================================
float Renderer::GetPerspectiveMultiple()
{
  return s_pRenderer->tanHalfVerticalFov * (GetScreenHeight() / 2);
}

//==============================================================================
void Renderer::SetViewMatrix( const Matrix& mView)
{
  const CIwFMat*  pMat(reinterpret_cast<const CIwFMat*>(&mView));
  IwGxSetViewMatrix(pMat);
}

//==============================================================================
void Renderer::SetModelMatrix( const Matrix& mModel)
{
  Matrix  m(mModel);
  m.Transpose();

  const CIwFMat*  pMat(reinterpret_cast<const CIwFMat*>(&m));
  IwGxSetModelMatrix(pMat);
}

//==============================================================================
void  Renderer::GetViewMatrix(Matrix& m)
{
  memcpy(&m, &IwGxGetViewMatrix(), sizeof(CIwFMat));
}

//==============================================================================
void  Renderer::GetModelMatrix(Matrix& m)
{
  CIwFMat&  mModel(IwGxGetModelMatrix());
  memcpy(&m, &mModel, sizeof(CIwFMat));
  m.Transpose();
}

//==============================================================================
void Renderer::SetMaterial( Material* pMat )
{
  IwGxSetMaterial(static_cast<CIwMaterial*>(pMat->GetImpl()));
}

//==============================================================================
void  Renderer::SetVertStream(FloatBuffer& fb)
{
  XR_ASSERT(Renderer::SetVertStream, fb.GetElementSize() == sizeof(Vector3)); // stub
  s_pRenderer->numVertices = fb.GetNumElements();
  IwGxSetVertStream(static_cast<CIwFVec3*>(fb.GetRaw()), fb.GetNumElements());
}

//==============================================================================
void  Renderer::SetUVStream(FloatBuffer& fb, int id)
{
  XR_ASSERT(Renderer::SetVertStream, fb.GetElementSize() == sizeof(Vector2) ||
    fb.GetNumElements() == 0);
  IwGxSetUVStream(static_cast<CIwFVec2*>(fb.GetRaw()), id);
}

//==============================================================================
void  Renderer::SetColStream(FloatBuffer& fb)
{
  XR_ASSERT(Renderer::SetVertStream, fb.GetElementSize() == sizeof(Color) ||
    fb.GetNumElements() == 0);
  IwGxSetColStream(static_cast<CIwColour*>(fb.GetRaw()), fb.GetNumElements());
}

//==============================================================================
void  Renderer::SetNormStream(FloatBuffer& fb)
{
  XR_ASSERT(Renderer::SetVertStream, fb.GetElementSize() == sizeof(Vector3) ||
    fb.GetNumElements() == 0);
  IwGxSetNormStream(static_cast<CIwFVec3*>(fb.GetRaw()), fb.GetNumElements());
}

//==============================================================================
void Renderer::SetClearColor(Color c)
{
  IwGxSetColClear(uint8(c.r * 0xff), uint8(c.g * 0xff), uint8(c.b * 0xff), uint8(c.a * 0xff));
}

//==============================================================================
void Renderer::SetAmbientColor(Color c)
{
  IwGxSetColAmbient(uint8(c.r * 0xff), uint8(c.g * 0xff), uint8(c.b * 0xff), uint8(c.a * 0xff));
}

//==============================================================================
Color Renderer::GetClearColor()
{
  return Color(IwGxGetColClear().Get());
}

//==============================================================================
Color Renderer::GetAmbientColor()
{
  return Color(IwGxGetColAmbient().Get());
}

//==============================================================================
void Renderer::SetScissorRect(const Rect& r)
{
  memcpy(&s_pRenderer->scissorRect, &r, sizeof(Rect));
  IwGxSetScissorScreenSpace(r.x, r.y, r.w, r.h);
}

//==============================================================================
void Renderer::ClearScissorRect()
{
  SetScissorRect(Rect(0, 0, GetScreenWidth(), GetScreenHeight()));
}

//==============================================================================
Rect Renderer::GetScissorRect()
{
  return s_pRenderer->scissorRect;
}

//==============================================================================
static const IwGxPrimType arPrimTypeMappings[] =
{
  IW_GX_LINE_LIST,
  IW_GX_LINE_STRIP,
  IW_GX_TRI_LIST,
  IW_GX_TRI_STRIP,
  IW_GX_TRI_FAN,
};

//==============================================================================
void  Renderer::DrawPrims(PrimType pt)
{
  IwGxDrawPrims(arPrimTypeMappings[pt], 0, s_pRenderer->numVertices);
}

//==============================================================================
void  Renderer::DrawPrims(PrimType pt, const uint16* pInds, int numInds)
{
  IwGxDrawPrims(arPrimTypeMappings[pt], const_cast<uint16*>(pInds), numInds);
}

//==============================================================================
void  Renderer::PrintString(int16 x, int16 y, const char* pString)
{
  IwGxPrintString(x, y, pString);
}

//==============================================================================
void  Renderer::SetLighting(bool state)
{
  state ? IwGxLightingOn() : IwGxLightingOff();
}

//==============================================================================
void  Renderer::SetFog(bool state)
{
  state ? IwGxFogOn() : IwGxFogOff();
}

//==============================================================================
void  Renderer::SetFogColor(Color c)
{
  CIwColour col;
  col.Set(uint8(c.r * 0xff), uint8(c.g * 0xff), uint8(c.b * 0xff), uint8(c.a * 0xff));
  IwGxSetFogCol(col);
}

//==============================================================================
void  Renderer::SetFogRange(float zFar, float zNear)
{
  IwGxSetFogFarZNearZ(zFar, zNear);
}

//==============================================================================
void Renderer::ClearBuffer(uint32 flags)
{
  uint32  flag2(((flags & BF_COLOR) ? IW_GX_COLOUR_BUFFER_F : 0 ) |
    ((flags & BF_DEPTH) ? IW_GX_DEPTH_BUFFER_F : 0 ));
  IwGxClear(flag2);
}

//==============================================================================
void Renderer::Flush()
{
  IwGxFlush();
  ++s_pRenderer->flushId;
}

//==============================================================================
void Renderer::Present()
{
  IwGxSwapBuffers();
}

//==============================================================================
uint32  Renderer::GetFlushId()
{
  return s_pRenderer->flushId;
}

} // XR
