#ifndef XR_SPRITERENDERER_HPP
#define XR_SPRITERENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Sprite.hpp"
#include "Matrix.hpp"
#include "xr/fundamentals.hpp"
#include "xr/Pool.hpp"
#include "xr/SharedPoolAllocator.hpp"
#include <list>

namespace xr
{

//==============================================================================
class SpriteRenderer
{
  XR_NONCOPY_DECL(SpriteRenderer)

public:
  // types
  using Vertex = Vertex::Format<Vertex::Color0<Color>, SpriteVertexFormat>;

  // structors
  SpriteRenderer();
  ~SpriteRenderer();

  // general
  void  Init(int numRecords);

  void  SetView(Matrix mView, float zNear/*, float zFar*/);

  void  Add(const xr::Sprite* sprite, const Vector3& position, float s,
          float rz, bool billboard = true);
  void  Add(const xr::Sprite* sprite, const Vector3& position, float sx,
          float sy, float rz, bool billboard = true);

  void  Add(Color tint, const xr::Sprite* sprite, const Vector3& position,
          float s, float rz, bool billboard = true);
  void  Add(Color tint, const xr::Sprite* sprite, const Vector3& position,
          float sx, float sy, float rz, bool billboard = true);

  void  Render();

  void  Clear();

protected:
  // types
  struct  Record
  {
    static bool Compare(const Record& lhs, const Record& rhs)
    {
      return lhs.zTemp > rhs.zTemp;
    }

    Matrix            xForm;
    Color             tint;
    const xr::Sprite* pSprite;

    float             zTemp;
  };

  typedef std::list<Record, xr::SharedPoolAllocator<Record> > RecordList;

  // data
  int         m_capacity;
  xr::Pool    m_pool;
  RecordList* m_records;

  Matrix      m_mView;
  Vector3     m_vForward;
  float       m_zNear;
  //float       m_zFar;

  FloatBuffer m_vertices;
  Gfx::IndexBufferHandle m_ibo;
};

//==============================================================================
// implementation
//==============================================================================
inline
void  SpriteRenderer::Add(const xr::Sprite* pSprite, const Vector3& position,
        float s, float rz, bool billboard)
{
  Add(Color::s_default, pSprite, position, s, rz, billboard);
}

//==============================================================================
inline
void  SpriteRenderer::Add(const xr::Sprite* pSprite, const Vector3& position,
        float sx, float sy, float rz, bool billboard)
{
  Add(Color::s_default, pSprite, position, sx, sy, rz, billboard);
}

} // xr

#endif  //XR_SPRITERENDERER_HPP
