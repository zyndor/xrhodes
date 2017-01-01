//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "MaterialImpl.hpp"
#include "xrgl.hpp"
#include <XR/Renderer.hpp>
#include <XR/Device.hpp>
#include <XR/Pool.hpp>
#include <XR/ProjectionHelpers.hpp>

namespace XR
{

//==============================================================================
static struct
{
  // data
  bool                  initSuccess;
  SDL_Window*           pMainWindow;
  SDL_Renderer*         pRenderer;
  SDL_GLContext         pGlContext;
  SVector2              screenSize;
  SVector2              deviceSize;
  Matrix                mModel;
  Matrix                mView;
  float                 zFar;
  float                 zNear;
  float                 tanHalfVerticalFov;
  Rect                  scissorRect;
  Pool                  framePool;
  int32                 numVertices;
  int32                 numTexCoords;
  int32                 numColors;
  int32                 numNormals;
  uint32                flushId;
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
  m.ToGL(arData);
  
  XR_GL_CALL(glLoadMatrixf(arData));
}

//==============================================================================
void Renderer::Init()
{
  XR_ASSERTMSG(Renderer, !s_rendererImpl.initSuccess, ("Already initialised!"));

  int openGlVersionMajor = Device::GetConfigInt("GFX", "openGLVersionMajor", 3);
  int openGlVersionMinor = Device::GetConfigInt("GFX", "openGLVersionMinor", 1);
  int openGLUseCompatibilityProfile = Device::GetConfigInt("GFX", "openGLUseCompatibility", 0);

  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, openGlVersionMajor );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, openGlVersionMinor );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, openGLUseCompatibilityProfile > 0 ? 
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);

  std::string caption = Device::GetConfig("GFX", "caption");
  if (caption.empty())
  {
    caption = "XRhodes Application";
  }
    
  int   width(Device::GetConfigInt("GFX", "width", 800));
  int   height(Device::GetConfigInt("GFX", "height", 600));
  int   poolSize(Device::GetConfigInt("GFX", "framePoolSize", 128000));
  
  uint32 flags(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!bool(Device::GetConfigInt("GFX", "windowed", false)))
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }
  
  s_rendererImpl.pMainWindow = SDL_CreateWindow(caption.c_str(),
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height, flags);

  flags = SDL_RENDERER_ACCELERATED;
  if (Device::GetConfigInt("GFX", "vsync", false))
  {
    flags |= SDL_RENDERER_PRESENTVSYNC;
  }
  s_rendererImpl.pRenderer = SDL_CreateRenderer(s_rendererImpl.pMainWindow,
    -1, flags);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(s_rendererImpl.pRenderer, width, height);

  s_rendererImpl.pGlContext = SDL_GL_CreateContext(s_rendererImpl.pMainWindow);
  SDL_GL_MakeCurrent(s_rendererImpl.pMainWindow, s_rendererImpl.pGlContext);
  
  SDL_RendererInfo  info;
  SDL_GetRendererInfo(s_rendererImpl.pRenderer, &info);
  
  XR_ASSERT(Renderer, info.flags & SDL_RENDERER_ACCELERATED);
  XR_ASSERT(Renderer, info.flags & SDL_RENDERER_TARGETTEXTURE);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  
  s_rendererImpl.deviceSize = SVector2(dm.w, dm.h);

  SDL_GL_GetDrawableSize(s_rendererImpl.pMainWindow, &width, &height);
  s_rendererImpl.screenSize = SVector2(width, height); 
  
  // init OpenGL
  glewExperimental = GL_TRUE;
  XR_GL_CALL(glewInit());

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
  s_rendererImpl.framePool.SetBuffer(poolSize, true, 0);
  
  // renderstreams
  s_rendererImpl.numVertices = 0;
  s_rendererImpl.numColors = 0;
  s_rendererImpl.numTexCoords = 0;
  s_rendererImpl.numNormals = 0;

  // matrices
  SetPerspective(M_PI * .25f, 10.0f, 1000.0f);

  s_rendererImpl.mView = Matrix::s_identity;
  s_rendererImpl.mModel = Matrix::s_identity;
  UpdateModelViewMatrix();

  s_rendererImpl.initSuccess = true;
}

//==============================================================================
void Renderer::Exit()
{
  XR_ASSERTMSG(Renderer, s_rendererImpl.initSuccess, ("Not initialised."));
  
  SDL_GL_DeleteContext(s_rendererImpl.pGlContext);
  SDL_DestroyWindow(s_rendererImpl.pMainWindow);
  SDL_DestroyRenderer(s_rendererImpl.pRenderer);

  SDL_VideoQuit();

  s_rendererImpl.numVertices = -1;
  s_rendererImpl.numColors = -1;
  s_rendererImpl.numTexCoords = -1;
  s_rendererImpl.numNormals = -1;
  
  s_rendererImpl.framePool.Flush();
  s_rendererImpl.framePool.SetBuffer(0, true, 0);

  s_rendererImpl.initSuccess = false;
}

//==============================================================================
int32 Renderer::GetScreenWidth()
{
  return s_rendererImpl.screenSize.x;
}

//==============================================================================
int32 Renderer::GetScreenHeight()
{
  return s_rendererImpl.screenSize.y;
}

//==============================================================================
int32 Renderer::GetDeviceWidth()
{
  return s_rendererImpl.deviceSize.x;
}

//==============================================================================
int32 Renderer::GetDeviceHeight()
{
  return s_rendererImpl.deviceSize.y;
}

//==============================================================================
void* Renderer::Alloc( int32 bytes )
{
  XR_ASSERT(Renderer, bytes >= 0);
  return s_rendererImpl.framePool.Allocate(bytes);
}

//==============================================================================
Material* Renderer::AllocMaterial()
{
  void* pMem(Alloc(sizeof(MaterialImpl) + sizeof(Material)));
  MaterialImpl* pImpl(new (static_cast<MaterialImpl*>(pMem)) MaterialImpl());
  Material*     pMat(new (pImpl + 1) Material(pImpl));
  return pMat;
}

//==============================================================================
RenderStream* Renderer::AllocStream(RenderStream::Format fmt, int numElems)
{
  XR_ASSERT(Renderer, numElems >= 0);

  int   bufferBytes(RenderStream::CalculateByteSize(fmt, numElems));
  void* pMem(Alloc(sizeof(RenderStream) + bufferBytes));
  RenderStream* pStream(new (pMem) RenderStream(fmt, numElems,
    static_cast<RenderStream*>(pMem) + 1, false));
  return pStream;
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
  static_cast<MaterialImpl*>(pMat->GetImpl())->Apply();
}

//==============================================================================
void  Renderer::SetVertStream(RenderStream& rs)
{
  XR_ASSERT(Renderer::SetVertStream, rs.GetFormat() == RenderStream::F_VECTOR3);
  XR_GL_CALL(glEnableClientState(GL_VERTEX_ARRAY));
  XR_GL_CALL(glVertexPointer(3, GL_FLOAT, 0, rs.GetData()));
  s_rendererImpl.numVertices = rs.GetCapacity();
}

//==============================================================================
void  Renderer::SetUVStream(RenderStream& rs, int id)
{
  XR_ASSERT(Renderer::SetUVStream, rs.GetFormat() == RenderStream::F_VECTOR2 ||
    rs.GetCapacity() == 0);
  if (rs.GetCapacity() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  }
  XR_GL_CALL(glTexCoordPointer(2, GL_FLOAT, 0, rs.GetData()));
  s_rendererImpl.numTexCoords = rs.GetCapacity();
}

//==============================================================================
void  Renderer::SetColStream(RenderStream& rs)
{
  XR_ASSERT(Renderer::SetColStream, rs.GetFormat() == RenderStream::F_COLOR ||
    rs.GetCapacity() == 0); // stub
  if (rs.GetCapacity() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_COLOR_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  }
  XR_GL_CALL(glColorPointer(4, GL_FLOAT, 0, rs.GetData()));
  s_rendererImpl.numColors = rs.GetCapacity();
}

//==============================================================================
void  Renderer::SetNormStream(RenderStream& rs)
{
  XR_ASSERT(Renderer::SetNormStream, rs.GetFormat() == RenderStream::F_VECTOR3 ||
    rs.GetCapacity() == 0); // stub
  if (rs.GetCapacity() > 0)
  {
    XR_GL_CALL(glEnableClientState(GL_NORMAL_ARRAY));
  }
  else
  {
    XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
  }
  XR_GL_CALL(glNormalPointer(3, 0, rs.GetData()));
  s_rendererImpl.numNormals = rs.GetCapacity();
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
  GL_TRIANGLE_FAN,
};

//==============================================================================
void  Renderer::DrawPrims(PrimType pt)
{
  XR_ASSERT(Renderer, s_rendererImpl.numVertices > 0);
  XR_ASSERT(Renderer, s_rendererImpl.numColors == 0 ||
    s_rendererImpl.numColors >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numTexCoords == 0 ||
    s_rendererImpl.numTexCoords >= s_rendererImpl.numVertices);
  XR_ASSERT(Renderer, s_rendererImpl.numNormals == 0 ||
    s_rendererImpl.numNormals >= s_rendererImpl.numVertices);
  XR_GL_CALL(glDrawArrays(arPrimTypeMappings[pt], 0, s_rendererImpl.numVertices));

  XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
}

//==============================================================================
void  Renderer::DrawPrims( PrimType pt, const uint16* pInds, int numInds)
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
  //glDrawArrays(arPrimTypeMappings[pt], 0, numInds);
  if (pInds != 0)
  {
    XR_GL_CALL(glEnableClientState(GL_INDEX_ARRAY));
    XR_GL_CALL(glDrawElements(arPrimTypeMappings[pt], numInds,
      GL_UNSIGNED_SHORT, pInds));
  }
  else
  {
    XR_ASSERT(Renderer, numInds <= s_rendererImpl.numVertices);
    XR_GL_CALL(glDrawArrays(arPrimTypeMappings[pt], 0, numInds));
  }
  
  XR_GL_CALL(glDisableClientState(GL_INDEX_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_COLOR_ARRAY));
  XR_GL_CALL(glDisableClientState(GL_NORMAL_ARRAY));
}

//==============================================================================
void Renderer::PrintString( int16 x, int16 y, const char* pString )
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
void Renderer::ClearBuffer(uint32 flags)
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
  SDL_GL_SwapWindow(s_rendererImpl.pMainWindow);
  ++s_rendererImpl.flushId;
}

//==============================================================================
uint32  Renderer::GetFlushId()
{
  return s_rendererImpl.flushId;
}

} // XR
