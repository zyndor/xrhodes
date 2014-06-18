//
// Nuclear Heart Games
// XRhodes
//
// SpriteRenderer.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    06/08/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
// 
//==============================================================================
#if !defined XR_SPRITERENDERER_HPP
#define XR_SPRITERENDERER_HPP

#include <list>
#include "fundamentals.hpp"
#include "Sprite.hpp"
#include "Pool.hpp"
#include "SharedPoolAllocator.hpp"
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

  void  SetView(Matrix mView, float zNear, float zFar);
  void  SetFadeColor(Color c);

  void  Add(const XR::Sprite* pSprite, const Vector3& position, float s,
          float rz);
  void  Add(const XR::Sprite* pSprite, const Vector3& position, float sx,
          float sy, float rz);

  void  Add(Color tint, const XR::Sprite* pSprite, const Vector3& position,
          float s, float rz);
  void  Add(Color tint, const XR::Sprite* pSprite, const Vector3& position,
          float sx, float sy, float rz);

  virtual void  Render();

  void  Clear();

protected:
  // types
  struct  Record
  {
    static bool Compare(const Record& lhs, const Record& rhs)
    {
      return lhs.zTemp < rhs.zTemp;
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
  RecordList* m_pRecords;

  Matrix      m_mView;
  Vector3     m_vForward;
  float       m_zNear;
  float       m_zFar;
  float       m_depthRangeRcp;
  
  Color       m_fadeColor;
  bool        m_useFadeColor;

#if !defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  typedef std::vector<uint16> Uint16Array;
  
  Uint16Array m_indices;
#endif
};

//==============================================================================
// implementation
//==============================================================================
inline
void  SpriteRenderer::Add(const XR::Sprite* pSprite, const Vector3& position,
        float s, float rz)
{
  Add(Color::s_default, pSprite, position, s, rz);
}

//==============================================================================
inline
void  SpriteRenderer::Add(const XR::Sprite* pSprite, const Vector3& position,
        float sx, float sy, float rz)
{
  Add(Color::s_default, pSprite, position, sx, sy, rz);
}

} // XR

#endif  //XR_SPRITERENDERER_HPP