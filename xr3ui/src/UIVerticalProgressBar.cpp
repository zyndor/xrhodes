//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIVerticalProgressBar.hpp"
#include "xr/UIRenderer.hpp"

namespace xr
{

//==============================================================================
UIVerticalProgressBar::UIVerticalProgressBar()
: UIProgressBarBase(UIProgressBarBase::FD_NEGATIVE)
{}

//==============================================================================
UIVerticalProgressBar::~UIVerticalProgressBar()
{}

//==============================================================================
void UIVerticalProgressBar::Render(IUIRenderer& renderer) const
{
  bool  isFdPositive = m_fillDir == FD_POSITIVE;

  float hSprite = sprite.GetHalfHeight() * 2.0f;
  float hs = h / hSprite;

  float yMin = sprite.GetTopPadding() * hs;
  float yMax = h - sprite.GetBottomPadding() * hs;

  float percMin = yMin / h;
  float percMax = yMax / h;

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage <= percMax))
  {
    float perc = Clamp(m_percentage, percMin, percMax);
    float y1 = h * (isFdPositive ? perc : 1.0f - perc);

    float ws = w / (sprite.GetHalfWidth() * 2.0f);
    float left = x + sprite.GetLeftPadding() * ws;
    float right = x + w - sprite.GetRightPadding() * ws;

    auto spriteVerts = sprite.GetVertices();
    const float hRatio = hSprite / sprite.GetQuadHeight();
    float uv1 = (isFdPositive ? (perc - percMin) : (percMax - perc)) * hRatio;
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(spriteVerts[Quad::Vertex::SW].uv0.x, spriteVerts[Quad::Vertex::NW].uv0.x, uv1);
    }
    else
    {
      uv1 = Lerp(spriteVerts[Quad::Vertex::NW].uv0.y, spriteVerts[Quad::Vertex::SW].uv0.y, uv1);
    }

    auto verts = renderer.NewSprite(material);
    sprite.CopyUVsTo(verts);

    y1 += y;

    if (isFdPositive)
    {
      float top = y + yMin;

      if (sprite.IsUVRotated())
      {
        verts[Quad::Vertex::SW].uv0.x = uv1;
        verts[Quad::Vertex::SE].uv0.x = uv1;
      }
      else
      {
        verts[Quad::Vertex::SW].uv0.y = uv1;
        verts[Quad::Vertex::SE].uv0.y = uv1;
      }

      verts[Quad::Vertex::NW].pos = Vector3(left, top, .0f);
      verts[Quad::Vertex::SW].pos = Vector3(left, y1, .0f);
      verts[Quad::Vertex::SE].pos = Vector3(right, y1, .0f);
      verts[Quad::Vertex::NE].pos = Vector3(right, top, .0f);
    }
    else
    {
      float bottom = y + yMax;

      if (sprite.IsUVRotated())
      {
        verts[Quad::Vertex::SW].uv0.x = uv1;
        verts[Quad::Vertex::SE].uv0.x = uv1;
      }
      else
      {
        verts[Quad::Vertex::NW].uv0.y = uv1;
        verts[Quad::Vertex::NE].uv0.y = uv1;
      }

      verts[Quad::Vertex::NW].pos = Vector3(left, y1, .0f);
      verts[Quad::Vertex::SW].pos = Vector3(left, bottom, .0f);
      verts[Quad::Vertex::SE].pos = Vector3(right, bottom, .0f);
      verts[Quad::Vertex::NE].pos = Vector3(right, y1, .0f);
    }

    _ApplyColor(verts);
  }
}

} // xr
