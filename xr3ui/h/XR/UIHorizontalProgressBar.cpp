//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIHorizontalProgressBar.hpp"
#include "UIRenderer.hpp"

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
void UIHorizontalProgressBar::Render() const
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

    const RenderStream& rsSpriteUVs(sprite.GetUVs());
    const float wRatio(wSprite / sprite.GetQuadWidth());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * wRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(rsSpriteUVs.GetY(Sprite::VI_SW),
        rsSpriteUVs.GetY(Sprite::VI_SE), uv1);
    }
    else
    {
      uv1 = Lerp(rsSpriteUVs.GetX(Sprite::VI_NW),
        rsSpriteUVs.GetX(Sprite::VI_NE), uv1);
    }

    RenderStream* pRsVerts(Renderer::AllocStream(RenderStream::F_VECTOR3,
                    Sprite::kNumVertices));
    RenderStream* pRsUVs(Renderer::AllocStream(RenderStream::F_VECTOR2,
                    Sprite::kNumVertices));
    sprite.CopyUVsTo(0, *pRsUVs);

    x1 += x;

    if (isFdPositive)
    {
      float left(x + xMin);

      if (sprite.IsUVRotated())
      {
        pRsUVs->SetY(Sprite::VI_SE, uv1);
        pRsUVs->SetY(Sprite::VI_NE, uv1);
      }
      else
      {
        pRsUVs->SetX(Sprite::VI_SE, uv1);
        pRsUVs->SetX(Sprite::VI_NE, uv1);
      }

      pRsVerts->Set(Sprite::VI_NW, Vector3(left, top, .0f));
      pRsVerts->Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      pRsVerts->Set(Sprite::VI_SE, Vector3(x1, bottom, .0f));
      pRsVerts->Set(Sprite::VI_NE, Vector3(x1, top, .0f));
    }
    else
    {
      float right(x + xMax);

      if (sprite.IsUVRotated())
      {
        pRsUVs->SetY(Sprite::VI_NW, uv1);
        pRsUVs->SetY(Sprite::VI_SW, uv1);
      }
      else
      {
        pRsUVs->SetX(Sprite::VI_NW, uv1);
        pRsUVs->SetX(Sprite::VI_SW, uv1);
      }

      pRsVerts->Set(Sprite::VI_NW, Vector3(x1, top, .0f));
      pRsVerts->Set(Sprite::VI_SW, Vector3(x1, bottom, .0f));
      pRsVerts->Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      pRsVerts->Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }

    Renderer::SetMaterial(sprite.GetMaterial());
    Renderer::SetAmbientColor(color);

    Renderer::SetVertStream(*pRsVerts);
    Renderer::SetUVStream(*pRsUVs);

    Renderer::DrawPrims(PRIM_TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
  }
}

//==============================================================================
void UIHorizontalProgressBar::Render( UIRenderer* pRenderer ) const
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

    const RenderStream& rsSpriteUVs(sprite.GetUVs());
    const float wRatio(wSprite / sprite.GetQuadWidth());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * wRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(rsSpriteUVs.GetY(Sprite::VI_SW),
        rsSpriteUVs.GetY(Sprite::VI_SE), uv1);
    }
    else
    {
      uv1 = Lerp(rsSpriteUVs.GetX(Sprite::VI_NW),
        rsSpriteUVs.GetX(Sprite::VI_NE), uv1);
    }

    RenderStream  rsUVs;
    RenderStream  rsVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, rsUVs));
    sprite.CopyUVsTo(0, rsUVs);

    x1 += x;

    if (isFdPositive)
    {
      float left(x + xMin);

      if (sprite.IsUVRotated())
      {
        rsUVs.SetY(Sprite::VI_SE, uv1);
        rsUVs.SetY(Sprite::VI_NE, uv1);
      }
      else
      {
        rsUVs.SetX(Sprite::VI_SE, uv1);
        rsUVs.SetX(Sprite::VI_NE, uv1);
      }

      rsVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f));
      rsVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      rsVerts.Set(Sprite::VI_SE, Vector3(x1, bottom, .0f));
      rsVerts.Set(Sprite::VI_NE, Vector3(x1, top, .0f));
    }
    else
    {
      float right(x + xMax);

      if (sprite.IsUVRotated())
      {
        rsUVs.SetY(Sprite::VI_NW, uv1);
        rsUVs.SetY(Sprite::VI_SW, uv1);
      }
      else
      {
        rsUVs.SetX(Sprite::VI_NW, uv1);
        rsUVs.SetX(Sprite::VI_SW, uv1);
      }

      rsVerts.Set(Sprite::VI_NW, Vector3(x1, top, .0f));
      rsVerts.Set(Sprite::VI_SW, Vector3(x1, bottom, .0f));
      rsVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      rsVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
  }
}

} // XR
