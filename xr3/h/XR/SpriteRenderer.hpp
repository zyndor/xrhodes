//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SPRITERENDERER_HPP
#define XR_SPRITERENDERER_HPP

#include <list>
#include "XR/fundamentals.hpp"
#include "XR/Pool.hpp"
#include "XR/SharedPoolAllocator.hpp"
#include "Sprite.hpp"
#include "IndexMesh.hpp"

namespace XR
{

#define XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  
//==============================================================================
class SpriteRenderer
#if defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  : protected IndexMesh
#endif
{
  XR_NONCOPY_DECL(SpriteRenderer)

public:
  // structors
  SpriteRenderer();
  ~SpriteRenderer();

  // general
  void  Init(int numRecords);

  void  SetView(Matrix mView, float zNear/*, float zFar*/);

  void  Add(const XR::Sprite* sprite, const Vector3& position, float s,
          float rz, bool billboard = true);
  void  Add(const XR::Sprite* sprite, const Vector3& position, float sx,
          float sy, float rz, bool billboard = true);

  void  Add(Color tint, const XR::Sprite* sprite, const Vector3& position,
          float s, float rz, bool billboard = true);
  void  Add(Color tint, const XR::Sprite* sprite, const Vector3& position,
          float sx, float sy, float rz, bool billboard = true);

  virtual void  Render();

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
    const XR::Sprite* pSprite;

    float             zTemp;
  };

  typedef std::list<Record, XR::SharedPoolAllocator<Record> > RecordList;

  // data
  int         m_capacity;
  XR::Pool    m_pool;
  RecordList* m_records;

  Matrix      m_mView;
  Vector3     m_vForward;
  float       m_zNear;
  //float       m_zFar;

#ifndef XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  typedef std::vector<uint16_t> Uint16Array;
  
  Uint16Array m_indices;
#endif
};

//==============================================================================
// implementation
//==============================================================================
inline
void  SpriteRenderer::Add(const XR::Sprite* pSprite, const Vector3& position,
        float s, float rz, bool billboard)
{
  Add(Color::s_default, pSprite, position, s, rz, billboard);
}

//==============================================================================
inline
void  SpriteRenderer::Add(const XR::Sprite* pSprite, const Vector3& position,
        float sx, float sy, float rz, bool billboard)
{
  Add(Color::s_default, pSprite, position, sx, sy, rz, billboard);
}

} // XR

#endif  //XR_SPRITERENDERER_HPP