//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIHorizontalProgressBar.hpp"
#include "XR/UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIHorizontalProgressBar::UIHorizontalProgressBar()
: UIProgressBarBase(UIProgressBarBase::FD_POSITIVE)
{}

//==============================================================================
UIHorizontalProgressBar::~UIHorizontalProgressBar()
{}

//==============================================================================
void UIHorizontalProgressBar::Render(IUIRenderer& renderer) const
{
  bool  isFdPositive(m_fillDir == FD_POSITIVE);

  float wSprite(sprite.GetHalfWidth() * 2.0f);
  float ws(w / wSprite);

  float xMin(sprite.GetLeftPadding() * ws);
  float xMax(w - sprite.GetRightPadding() * ws);

  float percMin(xMin / w);
  float percMax(xMax / w);

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage <= percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    float x1(w * (isFdPositive ? perc : 1.0f - perc));

    float hs(h / (sprite.GetHalfHeight() * 2.0f));
    float top(y + sprite.GetTopPadding() * hs);
    float bottom(y + h - sprite.GetBottomPadding() * hs);

    auto spriteVerts = sprite.GetVertices();
    const float wRatio(wSprite / sprite.GetQuadWidth());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * wRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(spriteVerts[Sprite::VI_SW].uv0.y, spriteVerts[Sprite::VI_SE].uv0.y, uv1);
    }
    else
    {
      uv1 = Lerp(spriteVerts[Sprite::VI_NW].uv0.x, spriteVerts[Sprite::VI_NE].uv0.x, uv1);
    }

    auto verts = renderer.NewSprite(material);
    sprite.CopyUVsTo(verts);

    x1 += x;

    if (isFdPositive)
    {
      float left(x + xMin);

      if (sprite.IsUVRotated())
      {
        verts[Sprite::VI_SE].uv0.y = uv1;
        verts[Sprite::VI_NE].uv0.y = uv1;
      }
      else
      {
        verts[Sprite::VI_SE].uv0.x = uv1;
        verts[Sprite::VI_NE].uv0.x = uv1;
      }

      verts[Sprite::VI_NW].pos = Vector3(left, top, .0f);
      verts[Sprite::VI_SW].pos = Vector3(left, bottom, .0f);
      verts[Sprite::VI_SE].pos = Vector3(x1, bottom, .0f);
      verts[Sprite::VI_NE].pos = Vector3(x1, top, .0f);
    }
    else
    {
      float right(x + xMax);

      if (sprite.IsUVRotated())
      {
        verts[Sprite::VI_NW].uv0.y = uv1;
        verts[Sprite::VI_SW].uv0.y = uv1;
      }
      else
      {
        verts[Sprite::VI_NW].uv0.x = uv1;
        verts[Sprite::VI_SW].uv0.x = uv1;
      }

      verts[Sprite::VI_NW].pos = Vector3(x1, top, .0f);
      verts[Sprite::VI_SW].pos = Vector3(x1, bottom, .0f);
      verts[Sprite::VI_SE].pos = Vector3(right, bottom, .0f);
      verts[Sprite::VI_NE].pos = Vector3(right, top, .0f);
    }

    _ApplyColor(verts);
  }
}

} // XR
