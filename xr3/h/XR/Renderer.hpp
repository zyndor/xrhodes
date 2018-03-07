#ifndef XR_RENDERER_HPP
#define XR_RENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Rect.hpp"
#include "Material.hpp"
#include "FloatBuffer.hpp"
#include "PrimType.hpp"
#include "Ray.hpp"
#include "Matrix.hpp"
#include "XR/fundamentals.hpp"
#include "XR/Callback.hpp"

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
  static void           Init(void* mainWindow);
  static void           Exit();

  static void*          Alloc(size_t bytes);
  static Material*      AllocMaterial();
  static FloatBuffer*   AllocBuffer(uint32_t elemSize, uint32_t numElems);

  static void           SetVertStream(FloatBuffer const& fb);
  static void           SetUVStream(FloatBuffer const& fb = FloatBuffer(), int id = 0);
  static void           SetColStream(FloatBuffer const& fb = FloatBuffer());
  static void           SetNormStream(FloatBuffer const& fb = FloatBuffer());

  static void           DrawPrims(PrimType pt);
  static void           DrawPrims(PrimType pt, const uint16_t* pInds, int numInds);

  static uint32_t       GetFlushId();
};

} // XR

//==============================================================================
#define XR_RENDERER_ALLOC(c, n) static_cast<c*>(XR::Renderer::Alloc(sizeof(c) * (n)))

#endif // XR_RENDERER_HPP
