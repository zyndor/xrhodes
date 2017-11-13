//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xrgl.hpp"
#include "XR/Renderer.hpp"
#include "XR/Device.hpp"
#include "XR/Pool.hpp"
#include "XR/ProjectionHelpers.hpp"
#include "XR/Mat4x4Helper.hpp"
#include "XR/SVector2.hpp"
#include "XR/debug.hpp"
#include "SDL.h"

#ifdef XR_GL_CALL
#undef XR_GL_CALL
#endif
#define XR_GL_CALL(x) // work around legacy OpenGL until we can rip Renderer out completely.

namespace XR
{

//==============================================================================
static struct
{
  // data
  bool                  initSuccess;
  SDL_Window*           mainWindow; // no ownership
  SDL_Renderer*         pRenderer;  // yes ownership
  SDL_GLContext         glContext;  // yes ownership
  SVector2              screenSize;
  SVector2              deviceSize;
  Matrix                mModel;
  Matrix                mView;
  float                 zFar;
  float                 zNear;
  float                 tanHalfVerticalFov;
  Rect                  scissorRect;
  Pool                  framePool;
  int32_t               numVertices;
  int32_t               numTexCoords;
  int32_t               numColors;
  int32_t               numNormals;
  uint32_t              flushId;
} s_rendererImpl;

//==============================================================================
static void UpdateModelViewMatrix()
{
  XR_GL_CALL(glMatrixMode(GL_MODELVIEW));
  XR_GL_CALL(glLoadIdentity());

  static float arData[Renderer::kNumPersMatrixElems];
  Matrix  m(s_rendererImpl.mModel);
  m.RotateBy(s_rendererImpl.mView);
  m.t = s_rendererImpl.mView.RotateVec(m.t - s_rendererImpl.mView.t);
  Mat4x4Helper::ExportToGL(m, arData);
  
  XR_GL_CALL(glLoadMatrixf(arData));
}

//==============================================================================
void Renderer::Init(void* mainWindow)
{
  XR_ASSERTMSG(Renderer, !s_rendererImpl.initSuccess, ("Already initialised!"));
  XR_ASSERTMSG(Renderer, mainWindow, ("Main window must not be null!"));

  int openGlVersionMajor = Device::GetConfigInt("GFX", "openGLVersionMajor", 3);
  int openGlVersionMinor = Device::GetConfigInt("GFX", "openGLVersionMinor", 1);
  int openGLUseCompatibilityProfile = Device::GetConfigInt("GFX", "openGLUseCompatibility", 0);

  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, openGlVersionMajor );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, openGlVersionMinor );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, openGLUseCompatibilityProfile > 0 ? 
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);

  // Window must be created at this point. In SDL, we do it in Device.
  uint32_t flags = SDL_RENDERER_ACCELERATED;
  if (Device::GetConfigInt("Display", "vsync", false))
  {
    flags |= SDL_RENDERER_PRESENTVSYNC;
  }

  s_rendererImpl.mainWindow = static_cast<SDL_Window*>(mainWindow);
  s_rendererImpl.pRenderer = SDL_CreateRenderer(s_rendererImpl.mainWindow, -1,
    flags);

  int width, height;
  SDL_GL_GetDrawableSize(s_rendererImpl.mainWindow, &width, &height);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(s_rendererImpl.pRenderer, width, height);

  s_rendererImpl.glContext = SDL_GL_CreateContext(s_rendererImpl.mainWindow);
  SDL_GL_MakeCurrent(s_rendererImpl.mainWindow, s_rendererImpl.glContext);
  
  SDL_RendererInfo  info;
  SDL_GetRendererInfo(s_rendererImpl.pRenderer, &info);
  
  XR_ASSERT(Renderer, info.flags & SDL_RENDERER_ACCELERATED);
  XR_ASSERT(Renderer, info.flags & SDL_RENDERER_TARGETTEXTURE);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  
  s_rendererImpl.deviceSize = SVector2(dm.w, dm.h);
  s_rendererImpl.screenSize = SVector2(width, height); 
  
  // init OpenGL

  XR_GL_CALL(glViewport(0, 0, (GLsizei)s_rendererImpl.screenSize.x,
    (GLsizei)s_rendererImpl.screenSize.y));

  XR_GL_CALL(glClearColor(.0f, .0f, .0f, .0f));
  XR_GL_CALL(glClearDepth(1.0f));

  XR_GL_CALL(glEnable(GL_DEPTH_TEST));
  XR_GL_CALL(glDepthFunc(GL_LEQUAL));

  // scissor box
  GLint arVal[4];
  XR_GL_CALL(glGetIntegerv(GL_SCISSOR_BOX, (GLint*)&arVal));

  s_rendererImpl.scissorRect.x = arVal[0];
  s_rendererImpl.scissorRect.y = arVal[1];
  s_rendererImpl.scissorRect.w = arVal[2];
  s_rendererImpl.scissorRect.h = arVal[3];

  // initialise frame pool
  int   poolSize(Device::GetConfigInt("GFX", "framePoolSize", 128000));
  s_rendererImpl.framePool.SetBuffer(poolSize, true, 0);
  
  // FloatBuffers
  s_rendererImpl.numVertices = 0;
  s_rendererImpl.numColors = 0;
  s_rendererImpl.numTexCoords = 0;
  s_rendererImpl.numNormals = 0;

  // matrices
  SetPerspective(M_PI * .25f, 10.0f, 1000.0f);

  s_rendererImpl.mView = Matrix::Identity();
  s_rendererImpl.mModel = Matrix::Identity();
  UpdateModelViewMatrix();

  s_rendererImpl.initSuccess = true;
}

//==============================================================================
void Renderer::Exit()
{
  XR_ASSERTMSG(Renderer, s_rendererImpl.initSuccess, ("Not initialised."));
  
  SDL_GL_DeleteContext(s_rendererImpl.glContext);
  SDL_DestroyRenderer(s_rendererImpl.pRenderer);

  s_rendererImpl.numVertices = -1;
  s_rendererImpl.numColors = -1;
  s_rendererImpl.numTexCoords = -1;
  s_rendererImpl.numNormals = -1;
  
  s_rendererImpl.framePool.Flush();
  s_rendererImpl.framePool.SetBuffer(0, true, 0);

  s_rendererImpl.initSuccess = false;
}

//==============================================================================
int32_t Renderer::GetScreenWidth()
{
  return s_rendererImpl.screenSize.x;
}

//==============================================================================
int32_t Renderer::GetScreenHeight()
{
  return s_rendererImpl.screenSize.y;
}

//==============================================================================
int32_t Renderer::GetDeviceWidth()
{
  return s_rendererImpl.deviceSize.x;
}

//==============================================================================
int32_t Renderer::GetDeviceHeight()
{
  return s_rendererImpl.deviceSize.y;
}

//==============================================================================
void* Renderer::Alloc(size_t bytes)
{
  XR_ASSERT(Renderer, bytes >= 0);
  return s_rendererImpl.framePool.Allocate(bytes);
}

//==============================================================================
Material* Renderer::AllocMaterial()
{
  XR_ASSERTMSG(Renderer, false, ("Renderer is being deprecated imminently."));
  //void* pMem(Alloc(sizeof(MaterialImpl) + sizeof(Material)));
  //MaterialImpl* pImpl(new (static_cast<MaterialImpl*>(pMem)) MaterialImpl());
  //Material*     pMat(new (pImpl + 1) Material(pImpl));
  //return pMat;
  return nullptr;
}

//==============================================================================
FloatBuffer* Renderer::AllocBuffer(size_t elemSize, size_t numElems)
{
  XR_ASSERT(Renderer, numElems >= 0);

  size_t bufferBytes(elemSize * numElems);
  void* pMem(Alloc(sizeof(FloatBuffer) + bufferBytes));
  FloatBuffer* pBuffer(new (pMem) FloatBuffer());
  pBuffer->SetBuffer(elemSize, numElems, reinterpret_cast<float*>(pBuffer + 1));
  return pBuffer;
}

//==============================================================================
void Renderer::SetPerspMatrix(const float (&arData)[kNumPersMatrixElems])
{
  XR_ASSERT(Renderer, arData != 0);
  XR_GL_CALL(glMatrixMode(GL_PROJECTION));
  if(arData)
  {
    float arPerspMatrix[kNumPersMatrixElems];
    memcpy(arPerspMatrix, arData, sizeof(arPerspMatrix));
    arPerspMatrix[5] *= -1.0f;

    XR_GL_CALL(glLoadMatrixf(arPerspMatrix));
  }
  else
  {
    XR_GL_CALL(glLoadIdentity());
  }
}

//==============================================================================
void  Renderer::SetOrtho(float left, float right, float bottom, float top,
    float zNear, float zFar)
{
  XR_GL_CALL(glMatrixMode(GL_PROJECTION));
  
  s_rendererImpl.zFar = zFar;
  if (zNear == .0f)
  {
    zNear = -zFar;
  }
  s_rendererImpl.zNear = zNear;
  s_rendererImpl.tanHalfVerticalFov = .0f;
  
  float arPerspMatrix[kNumPersMatrixElems];
  ProjectionHelpers::CalculateOrtho(left, right, bottom, top, zNear, zFar,
    arPerspMatrix);
  SetPerspMatrix(arPerspMatrix);
}

//==============================================================================
void  Renderer::SetPerspective(float verticalFov, float aspectRatio, float zNear, float zFar)
{
  XR_ASSERT(Renderer, zNear < zFar);
  XR_ASSERT(Renderer, aspectRatio > .0f);
  XR_GL_CALL(glMatrixMode(GL_PROJECTION));

  s_rendererImpl.zNear = zNear;
  s_rendererImpl.zFar = zFar;
  
//  float f(1.0f / tanf(verticalFov * .5f));
//  float dz(1.0f / (zFar - zNear));
//  float arData[16] =
//  {
//    f, .0f, .0f, .0f,
//    .0f, f / aspectRatio, .0f, .0f,
//    .0f, .0f, (zNear + zFar) * dz, (2.0f * zNear * zFar) * dz,
//    .0f, .0f, 1.0f, .0f      
//  };

  float arPerspMatrix[kNumPersMatrixElems];
  ProjectionHelpers::CalculatePerspective(verticalFov, aspectRatio, zNear, zFar,
    arPerspMatrix, &s_rendererImpl.tanHalfVerticalFov);
  SetPerspMatrix(arPerspMatrix);
}

//==============================================================================
void  Renderer::SetPerspective(float verticalFov, float zNear, float zFar)
{
  SetPerspective(verticalFov, float(GetScreenWidth()) / float(GetScreenHeight()), zNear, zFar);
}

//==============================================================================
void Renderer::SetFarNearZ( float zFar, float zNear )
{
  XR_GL_CALL(glDepthRange(zNear, zFar));
  s_rendererImpl.zFar = zFar;
  s_rendererImpl.zNear = zNear;
}

//==============================================================================
float Renderer::GetNearZ()
{
  return s_rendererImpl.zNear;
}

//==============================================================================
float Renderer::GetFarZ()
{
  return s_rendererImpl.zFar;
}

//==============================================================================
float Renderer::GetPerspectiveMultiple()
{
  return s_rendererImpl.tanHalfVerticalFov * (GetScreenHeight() / 2);
}

//==============================================================================
void Renderer::SetViewMatrix(const Matrix& m)
{
  s_rendererImpl.mView = m;
  s_rendererImpl.mView.zx *= -1.0f;
  s_rendererImpl.mView.zy *= -1.0f;
  s_rendererImpl.mView.zz *= -1.0f;
  s_rendererImpl.mView.Transpose();
  UpdateModelViewMatrix();
}

//==============================================================================
void Renderer::SetModelMatrix(const Matrix& m)
{
  s_rendererImpl.mModel = m;
  UpdateModelViewMatrix();
}

//==============================================================================
void  Renderer::GetViewMatrix(Matrix& m)
{
  m = s_rendererImpl.mView;
  m.Transpose();
  m.zx *= -1.0f;
  m.zy *= -1.0f;
  m.zz *= -1.0f;
}

//==============================================================================
void  Renderer::GetModelMatrix(Matrix& m)
{
  m = s_rendererImpl.mModel;
}

//==============================================================================
void Renderer::SetMaterial( Material* pMat )
{
  XR_ASSERTMSG(Renderer, false, ("Renderer is being deprecated imminently."));
  //static_cast<MaterialImpl*>(pMat->GetImpl())->Apply();
}

//==============================================================================
void  Renderer::SetVertStream(FloatBuffer const& fb)
{
  XR_ASSERT(Renderer::SetVertStream, fb.GetElementSizeBytes() == sizeof(Vector3));
  XR_GL_CALL(glEnableClientState(GL_VERTEX_ARRAY));
  XR_GL_CALL(glVertexPointer(3, GL_FLOAT, 0, fb.GetRaw()));
  s_rendererImpl.numVertices = fb.GetNumElements();
}

//==============================================================================
void  Renderer::SetUVStream(FloatBuffer const& fb, int id)
{
  XR_ASSERT(Renderer::SetUVStream, fb.GetElementSizeBytes() == sizeof(Vector2) ||
    fb.GetNumElements() == 0);
  if (fb.GetNumElements() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  }
  XR_GL_CALL(glTexCoordPointer(2, GL_FLOAT, 0, fb.GetRaw()));
  s_rendererImpl.numTexCoords = fb.GetNumElements();
}

//==============================================================================
void  Renderer::SetColStream(FloatBuffer const& fb)
{
  XR_ASSERT(Renderer::SetColStream, fb.GetElementSizeBytes() == sizeof(Color) ||
    fb.GetNumElements() == 0);
  if (fb.GetNumElements() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_COLOR_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  }
  XR_GL_CALL(glColorPointer(4, GL_FLOAT, 0, fb.GetRaw()));
  s_rendererImpl.numColors = fb.GetNumElements();
}

//==============================================================================
void  Renderer::SetNormStream(FloatBuffer const& fb)
{
  XR_ASSERT(Renderer::SetNormStream, fb.GetElementSizeBytes() == sizeof(Vector3) ||
    fb.GetNumElements() == 0);
  if (fb.GetNumElements() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_NORMAL_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
  }
  XR_GL_CALL(glNormalPointer(3, 0, fb.GetRaw()));
  s_rendererImpl.numNormals = fb.GetNumElements();
}

//==============================================================================
void Renderer::SetClearColor( Color c )
{
  XR_GL_CALL(glClearColor(c.r, c.g, c.b, c.a));
}

//==============================================================================
void Renderer::SetAmbientColor( Color c )
{
  XR_GL_CALL(glColor4f(c.r, c.g, c.b, c.a));
}

//==============================================================================
Color Renderer::GetClearColor()
{
  GLint arParam[4];
  XR_GL_CALL(glGetIntegerv(GL_COLOR_CLEAR_VALUE, arParam));
  return Color(arParam[0], arParam[1], arParam[2], arParam[3]);
}

//==============================================================================
Color Renderer::GetAmbientColor()
{
  GLint arParam[4];
  XR_GL_CALL(glGetIntegerv(GL_CURRENT_COLOR, arParam));
  return Color(arParam[0], arParam[1], arParam[2], arParam[3]);
}

//==============================================================================
void Renderer::SetScissorRect( const Rect& r )
{
  memcpy(&s_rendererImpl.scissorRect, &r, sizeof(Rect));
  XR_GL_CALL(glEnable(GL_SCISSOR_TEST));
  XR_GL_CALL(glScissor(r.x, r.y, r.w, r.h));
}

//==============================================================================
void Renderer::ClearScissorRect()
{
  Rect  r(0, 0, GetScreenWidth(), GetScreenHeight());
  SetScissorRect(r);
  XR_GL_CALL(glDisable(GL_SCISSOR_TEST));
}

//==============================================================================
Rect Renderer::GetScissorRect()
{
  return s_rendererImpl.scissorRect;
}

//==============================================================================
static const GLenum arPrimTypeMappings[] =
{
  GL_LINES,
  GL_LINE_STRIP,
  GL_TRIANGLES,
  GL_TRIANGLE_STRIP,
  GL_QUADS,
};

//==============================================================================
void  Renderer::DrawPrims(PrimType prim)
{
  XR_ASSERT(Renderer, s_rendererImpl.numVertices > 0);
  XR_ASSERT(Renderer, s_rendererImpl.numColors == 0 ||
    s_rendererImpl.numColors >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numTexCoords == 0 ||
    s_rendererImpl.numTexCoords >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numNormals == 0 ||
    s_rendererImpl.numNormals >= s_rendererImpl.numVertices);
  XR_GL_CALL(glDrawArrays(arPrimTypeMappings[int(prim)], 0, s_rendererImpl.numVertices));

  XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
}

//==============================================================================
void  Renderer::DrawPrims( PrimType prim, const uint16_t* pInds, int numInds)
{
  XR_ASSERT(Renderer, numInds >= 0);
  XR_ASSERT(Renderer, s_rendererImpl.numVertices > 0);
  XR_ASSERT(Renderer, s_rendererImpl.numColors == 0 ||
    s_rendererImpl.numColors >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numTexCoords == 0 ||
    s_rendererImpl.numTexCoords >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numNormals == 0 ||
    s_rendererImpl.numNormals >= s_rendererImpl.numVertices);
  //glIndexPointer(GL_UNSIGNED_SHORT, 0, pInds);
  //glDrawArrays(arPrimTypeMappings[prim], 0, numInds);
  if (pInds != 0)
  {
    XR_GL_CALL(glEnableClientState(GL_INDEX_ARRAY));
    XR_GL_CALL(glDrawElements(arPrimTypeMappings[int(prim)], numInds,
      GL_UNSIGNED_SHORT, pInds));
  }
  else
  {
    XR_ASSERT(Renderer, numInds <= s_rendererImpl.numVertices);
    XR_GL_CALL(glDrawArrays(arPrimTypeMappings[int(prim)], 0, numInds));
  }
  
  XR_GL_CALL(glDisableClientState(GL_INDEX_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
}

//==============================================================================
void Renderer::PrintString( int16_t x, int16_t y, const char* pString )
{
  //IwGxPrintString(x, y, pString);
}

//==============================================================================
void  Renderer::SetLighting(bool state)
{
  XR_GL_CALL(state ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING));
}

//==============================================================================
void  Renderer::SetFog(bool state)
{
  XR_GL_CALL(state ? glEnable(GL_FOG) : glDisable(GL_FOG));
}

//==============================================================================
void  Renderer::SetFogColor(Color c)
{
  XR_GL_CALL(glFogfv(GL_FOG_COLOR, c.arData));
}

//==============================================================================
void  Renderer::SetFogRange(float zFar, float zNear)
{
  XR_GL_CALL(glFogf(GL_FOG_START, zNear));
  XR_GL_CALL(glFogf(GL_FOG_END, zFar));
}

//==============================================================================
void Renderer::ClearBuffer(uint32_t flags)
{
  GLbitfield  bitField(((flags & BF_COLOR) ? GL_COLOR_BUFFER_BIT: 0 ) |
    ((flags & BF_DEPTH) ? GL_DEPTH_BUFFER_BIT : 0 ));
  XR_GL_CALL(glClear(bitField));
  
  //s_rendererImpl.framePool.Flush();
}

//==============================================================================
void Renderer::Flush()
{
  XR_GL_CALL(glFlush());
  s_rendererImpl.framePool.Flush();
}

//==============================================================================
void Renderer::Present()
{
  Flush();
  SDL_GL_SwapWindow(s_rendererImpl.mainWindow);
  ++s_rendererImpl.flushId;
}

//==============================================================================
uint32_t  Renderer::GetFlushId()
{
  return s_rendererImpl.flushId;
}

} // XR
