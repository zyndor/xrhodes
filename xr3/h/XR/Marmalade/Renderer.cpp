#include <IwGx.h>
#include "Renderer.hpp"

namespace XR
{

//==============================================================================
struct  RendererImpl
{
  // data
  float           zFar;
  float           zNear;
  float           zView;
  float           tanVFovHalf;
  Rect            scissorRect;
  int             numVertices;
  bool            isPerspective;
  float           rayCastZNear;
  Matrix          rayCastView;
  uint32          flushId;
};

static RendererImpl* s_pRenderer(0);

//==============================================================================
void Renderer::Init()
{
  XR_ASSERTMSG(Renderer, s_pRenderer == 0, ("Already initialised!"));

  s_pRenderer = new RendererImpl;

  s3eSurfaceSetInt( S3E_SURFACE_DEVICE_ORIENTATION_LOCK, S3E_SURFACE_LANDSCAPE);
  IwGxTickUpdate(); 
  IwGxFlush(); 
  IwGxSwapBuffers();
  s3eSurfaceSetInt( S3E_SURFACE_DEVICE_ORIENTATION_LOCK,
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

  SetPerspective(M_PI * .25f, 10.0f, 1000.0f);
  GetViewMatrix(s_pRenderer->rayCastView);
  s_pRenderer->rayCastZNear = s_pRenderer->zNear;

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
void* Renderer::Alloc( int32 bytes )
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
RenderStream* Renderer::AllocStream(RenderStream::Format fmt, int numElems)
{
  IwAssert(Renderer, numElems >= 0);

  int   bufferBytes(RenderStream::CalculateByteSize(fmt, numElems));
  void* pMemStream(Alloc(sizeof(RenderStream) + bufferBytes));
  void* pMemBuffer(static_cast<char*>(pMemStream) + sizeof(RenderStream));
  
  return new (pMemStream) RenderStream(fmt, numElems, pMemBuffer, false);
}

//==============================================================================
void Renderer::SetPerspMatrix( const float arData[kNumPersMatrixElems] )
{
  IwGxSetPerspectiveMatrix(const_cast<float*>(arData));
}

//==============================================================================
void Renderer::SetOrtho(float left, float right, float bottom, float top,
  float zFar, float zNear)
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

  float dx(1.0f / (right - left));
  float dy(1.0f / (top - bottom));
  float dz(1.0f / (zFar - zNear));
  
  float arPerspMatrix[16] =
  {
    2.0f * dx, .0f, .0f, .0f,
    .0f, -2.0f * dy, .0f, .0f,
    .0f, .0f, -2.0f * dz, .0f,
    (left + right) * -dx, (top + bottom) * -dy, (zFar + zNear) * -dz, 1.0f
  };
  
  s_pRenderer->zNear = zNear;
  s_pRenderer->zFar = zFar;
  s_pRenderer->zView = .0f;
  //IwGxSetFarZNearZ(zFar, zNear);
  IwGxSetPerspectiveMatrix(arPerspMatrix);

  s_pRenderer->isPerspective = false;
}

//==============================================================================
void Renderer::SetPerspective(float vFov, float aspect, float zNear,
  float zFar)
{
  //uint32  flags(IwGxGetFlags());
  //XR_TRACE(Renderer, ("ortho: %d", (flags & IW_GX_ORTHO_PROJ_F) != 0));
  //flags &= ~IW_GX_ORTHO_PROJ_F;
  //IwGxSetFlags(flags);

  s_pRenderer->tanVFovHalf = tanf(vFov * .5f);
  float f(1.0f / s_pRenderer->tanVFovHalf);
  float dz(zNear - zFar);
  
  float arPerspMatrix[16] =
  {
    f / aspect, .0f, .0f, .0f,
    .0f, f, .0f, .0f,
    .0f, .0f, (zFar + zNear) / dz, -1.0f,
    .0f, .0f, (2.0f * zFar * zNear) / dz, .0f
  };

  s_pRenderer->zNear = zNear;
  s_pRenderer->zFar = zFar;
  s_pRenderer->zView = float(IwGxGetScreenHeight() / 2) / f;
  IwGxSetFarZNearZ(zFar, zNear);
  IwGxSetPerspectiveMatrix(arPerspMatrix);

  s_pRenderer->isPerspective = true;
  s_pRenderer->rayCastZNear = zNear;
}

//==============================================================================
void Renderer::SetPerspective(float vFov, float zNear, float zFar)
{
  SetPerspective(vFov, float(IwGxGetScreenWidth()) / float(IwGxGetScreenHeight()),
    zNear, zFar);
}

//==============================================================================
void Renderer::SetFarNearZ( float zFar, float zNear )
{
  IwAssert(Renderer, zFar > zNear);
  s_pRenderer->zFar = zFar;
  s_pRenderer->zNear = zNear;
}

//==============================================================================
void  Renderer::SetPerspMult(float pm)
{
  // stub, recalculate projection
  s_pRenderer->zView = pm;
  IwGxSetPerspMul(pm);
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
float Renderer::GetPerspMult()
{
  return s_pRenderer->zView;
}

//==============================================================================
void Renderer::SetViewMatrix( const Matrix& mView)
{
  const CIwFMat*  pMat(reinterpret_cast<const CIwFMat*>(&mView));
  IwGxSetViewMatrix(pMat);

  if(s_pRenderer->isPerspective)
  {
    s_pRenderer->rayCastView = mView;
  }
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
Ray Renderer::GetViewRay(int16 x, int16 y)
{
  Vector3 vForward(s_pRenderer->rayCastView.arRot + MZX);
//  vForward.Normalise();

  Vector3 vRight(s_pRenderer->rayCastView.arRot + MXX);
  vRight.Normalise();

  Vector3 vUp(s_pRenderer->rayCastView.arRot + MYX);
  vUp.Normalise();

  const int   wScr(GetScreenWidth());
  const int   hScr(GetScreenHeight());
  float hProj(s_pRenderer->rayCastZNear * s_pRenderer->tanVFovHalf);
  float wProj(hProj * (float(wScr) / float(hScr)));
  
  vUp *= hProj;
  vRight *= wProj;
  
  int wHalf(wScr / 2);
  int hHalf(hScr / 2);
  x -= wHalf;
  y -= hHalf;
  float fx(float(x) / wHalf);
  float fy(float(y) / hHalf);

  Ray r;
  r.position = s_pRenderer->rayCastView.t;
  Vector3 zNearHit(s_pRenderer->rayCastView.t +
    (vForward * s_pRenderer->rayCastZNear) + (vRight * fx) + (vUp * fy));
  r.direction = zNearHit - s_pRenderer->rayCastView.t;
  r.direction.Normalise();
  r.length = std::numeric_limits<float>::max();
 
  return r;
}

//==============================================================================
void Renderer::SetMaterial( Material* pMat )
{
  IwGxSetMaterial(static_cast<CIwMaterial*>(pMat->GetImpl()));
}

//==============================================================================
void  Renderer::SetVertStream(RenderStream& rs)
{
  IwAssert(Renderer::SetVertStream, rs.GetFormat() == RenderStream::F_VECTOR3); // stub
  s_pRenderer->numVertices = rs.GetCapacity();
  IwGxSetVertStream(static_cast<CIwFVec3*>(rs.GetData()), rs.GetCapacity());
}

//==============================================================================
void  Renderer::SetUVStream(RenderStream& rs, int id)
{
  IwAssert(Renderer::SetVertStream, rs.GetFormat() == RenderStream::F_VECTOR2 ||
    rs.GetCapacity() == 0);
  IwGxSetUVStream(static_cast<CIwFVec2*>(rs.GetData()), id);
}

//==============================================================================
void  Renderer::SetColStream(RenderStream& rs)
{
  IwAssert(Renderer::SetVertStream, rs.GetFormat() == RenderStream::F_COLOR ||
    rs.GetCapacity() == 0);
  IwGxSetColStream(static_cast<CIwColour*>(rs.GetData()), rs.GetCapacity());
}

//==============================================================================
void  Renderer::SetNormStream(RenderStream& rs)
{
  IwAssert(Renderer::SetVertStream, rs.GetFormat() == RenderStream::F_VECTOR3 ||
    rs.GetCapacity() == 0);
  IwGxSetNormStream(static_cast<CIwFVec3*>(rs.GetData()), rs.GetCapacity());
}

//==============================================================================
void Renderer::SetClearColor( Color c )
{
  IwGxSetColClear(c.r * 0xff, c.g * 0xff, c.b * 0xff, c.a * 0xff);
}

//==============================================================================
void Renderer::SetAmbientColor( Color c )
{
  IwGxSetColAmbient(c.r * 0xff, c.g * 0xff, c.b * 0xff, c.a * 0xff);
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
void Renderer::SetScissorRect( const Rect& r )
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
void  Renderer::DrawPrims( PrimType pt)
{
  IwGxDrawPrims(arPrimTypeMappings[pt], 0, s_pRenderer->numVertices);
}

//==============================================================================
void  Renderer::DrawPrims( PrimType pt, const uint16* pInds, int numInds)
{
  IwGxDrawPrims(arPrimTypeMappings[pt], const_cast<uint16*>(pInds), numInds);
}

//==============================================================================
void  Renderer::PrintString( int16 x, int16 y, const char* pString )
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
