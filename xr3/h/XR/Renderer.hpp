//
// Nuclear Heart Games
// XRhodes
//
// RenderStream.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/09/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_RENDERER_HPP
#define XR_RENDERER_HPP

#include "Callback.hpp"
#include "Rect.hpp"
#include "Material.hpp"
#include "RenderStream.hpp"
#include "PrimType.hpp"
#include "Ray.hpp"

namespace XR
{

//==============================================================================
class Renderer
{
  XR_NONOBJECT_DECL(Renderer)

public:
  // types
  enum  BufferFlags
  {
    BF_COLOR = 0x01,
    BF_DEPTH = 0x02
  };
  
  enum
  {
    kNumPersMatrixElems = 16
  };
  
  // static
  static void           Init();
  static void           Exit();
  
  static int32          GetScreenWidth();
  static int32          GetScreenHeight();
  
  static int32          GetDeviceWidth();
  static int32          GetDeviceHeight();
  
  static void*          Alloc(int32 bytes);
  static Material*      AllocMaterial();
  static RenderStream*  AllocStream(RenderStream::Format fmt, int numVerts);
  
  static void           SetPerspMatrix(const float arData[kNumPersMatrixElems]);

  static void           SetOrtho(float left, float right, float bottom,
                          float top, float zFar = 1000.0f, float zNear = .0f);
  static void           SetPerspective(float vFov, float aspect, float zNear,
                          float zFar);
  static void           SetPerspective(float vFov, float zNear, float zFar);

  static void           SetFarNearZ(float zFar, float zNear);
  static void           SetPerspMult(float perspMul);
  static float          GetNearZ();
  static float          GetFarZ();
  static float          GetPerspMult();  

  static void           SetViewMatrix(const Matrix& m);
  static void           SetModelMatrix(const Matrix& m);
  
  static void           GetViewMatrix(Matrix& m);
  static void           GetModelMatrix(Matrix& m);

  static Ray            GetViewRay(int16 x, int16 y);
  
  static void           SetMaterial(Material* pMat);

  static void           SetVertStream(RenderStream& rs);
  static void           SetUVStream(RenderStream& rs = RenderStream::s_zero, int id = 0);
  static void           SetColStream(RenderStream& rs = RenderStream::s_zero);
  static void           SetNormStream(RenderStream& rs = RenderStream::s_zero);

  static void           DrawPrims(PrimType pt);
  static void           DrawPrims(PrimType pt, const uint16* pInds, int numInds);
                  
  static void           PrintString(int16 x, int16 y, const char* pString);
  static void           SetClearColor(Color c);
  static void           SetAmbientColor(Color c);
  
  static Color          GetClearColor();
  static Color          GetAmbientColor();
  
  static Rect           GetScissorRect();
  static void           SetScissorRect( const Rect& r );
  static void           ClearScissorRect();

  static void           SetLighting(bool state);
  static void           SetFog(bool state);
  
  static void           ClearBuffer(uint32 flags = (BF_COLOR | BF_DEPTH));
  static void           Flush();
  static void           Present();

  static uint32         GetFlushId();
};

} // XR

//==============================================================================
#define XR_RENDERER_ALLOC(c, n) static_cast<c*>(XR::Renderer::Alloc(sizeof(c) * (n)))

#endif // XR_RENDERER_HPP