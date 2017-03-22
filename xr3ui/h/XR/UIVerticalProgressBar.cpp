//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIVerticalProgressBar.hpp"
#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIVerticalProgressBar::UIVerticalProgressBar()
: UIProgressBarBase(UIProgressBarBase::FD_NEGATIVE)
{}

//==============================================================================
UIVerticalProgressBar::~UIVerticalProgressBar()
{}

//==============================================================================
void UIVerticalProgressBar::Render() const
{
  bool  isFdPositive(m_fillDir == FD_POSITIVE);

  float hSprite(sprite.GetHalfHeight() * 2.0f);
  float hs(h / hSprite);

  float yMin(sprite.GetTopPadding() * hs);
  float yMax(h - sprite.GetBottomPadding() * hs);

  float percMin(yMin / h); 
  float percMax(yMax / h);

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage <= percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    float y1(h * (isFdPositive ? perc : 1.0f - perc));

    float ws(w / (sprite.GetHalfWidth() * 2.0f));
    float left(x + sprite.GetLeftPadding() * ws);
    float right(x + w - sprite.GetRightPadding() * ws);

    const FloatBuffer& fbSpriteUVs(sprite.GetUVs());
    const float hRatio(hSprite / sprite.GetQuadHeight());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * hRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).x,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x, uv1);
    }
    else
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).y,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y, uv1);
    }

    FloatBuffer* pFbVerts(Renderer::AllocBuffer(sizeof(Vector3),
      Sprite::kNumVertices));
    FloatBuffer* pFbUVs(Renderer::AllocBuffer(sizeof(Vector2),
      Sprite::kNumVertices));
    sprite.CopyUVsTo(0, *pFbUVs);

    y1 += y;

    if (isFdPositive)
    {
      int top(y + yMin);

      if (sprite.IsUVRotated())
      {
        pFbUVs->Get<Vector2>(Sprite::VI_SW).x = uv1;
        pFbUVs->Get<Vector2>(Sprite::VI_SE).x = uv1;
      }
      else
      {
        pFbUVs->Get<Vector2>(Sprite::VI_SW).y = uv1;
        pFbUVs->Get<Vector2>(Sprite::VI_SE).y = uv1;
      }

      pFbVerts->Set(Sprite::VI_NW, Vector3(left, top, .0f));
      pFbVerts->Set(Sprite::VI_SW, Vector3(left, y1, .0f));
      pFbVerts->Set(Sprite::VI_SE, Vector3(right, y1, .0f));
      pFbVerts->Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
    else
    {
      int bottom(y + yMax);

      if (sprite.IsUVRotated())
      {
        pFbUVs->Get<Vector2>(Sprite::VI_SW).x = uv1;
        pFbUVs->Get<Vector2>(Sprite::VI_SE).x = uv1;
      }
      else
      {
        pFbUVs->Get<Vector2>(Sprite::VI_NW).y = uv1;
        pFbUVs->Get<Vector2>(Sprite::VI_NE).y = uv1;
      }

      pFbVerts->Set(Sprite::VI_NW, Vector3(left, y1, .0f));
      pFbVerts->Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      pFbVerts->Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      pFbVerts->Set(Sprite::VI_NE, Vector3(right, y1, .0f));
    }

    Renderer::SetMaterial(sprite.GetMaterial());
    Renderer::SetAmbientColor(color);

    Renderer::SetVertStream(*pFbVerts);
    Renderer::SetUVStream(*pFbUVs);

    Renderer::DrawPrims(PRIM_TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
  }
}

//==============================================================================
void UIVerticalProgressBar::Render( UIRenderer* pRenderer ) const
{
  bool  isFdPositive(m_fillDir == FD_POSITIVE);

  float hSprite(sprite.GetHalfHeight() * 2.0f);
  float hs(h / hSprite);

  float yMin(sprite.GetTopPadding() * hs);
  float yMax(h - sprite.GetBottomPadding() * hs);

  float percMin(yMin / h); 
  float percMax(yMax / h);

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage <= percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    float y1(h * (isFdPositive ? perc : 1.0f - perc));

    float ws(w / (sprite.GetHalfWidth() * 2.0f));
    float left(x + sprite.GetLeftPadding() * ws);
    float right(x + w - sprite.GetRightPadding() * ws);

    const FloatBuffer&  fbSpriteUVs(sprite.GetUVs());
    const float hRatio(hSprite / sprite.GetQuadHeight());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * hRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).x,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x, uv1);
    }
    else
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).y,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y, uv1);
    }

    FloatBuffer  fbUVs;
    FloatBuffer  fbVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, fbUVs));
    sprite.CopyUVsTo(0, fbUVs);

    y1 += y;

    if (isFdPositive)
    {
      float top(y + yMin);

      if (sprite.IsUVRotated())
      {
        fbUVs.Get<Vector2>(Sprite::VI_SW).x = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_SE).x = uv1;
      }
      else
      {
        fbUVs.Get<Vector2>(Sprite::VI_SW).y = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_SE).y = uv1;
      }

      fbVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f));
      fbVerts.Set(Sprite::VI_SW, Vector3(left, y1, .0f));
      fbVerts.Set(Sprite::VI_SE, Vector3(right, y1, .0f));
      fbVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
    else
    {
      float bottom(y + yMax);

      if (sprite.IsUVRotated())
      {
        fbUVs.Get<Vector2>(Sprite::VI_SW).x = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_SE).x = uv1;
      }
      else
      {
        fbUVs.Get<Vector2>(Sprite::VI_NW).y = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_NE).y = uv1;
      }

      fbVerts.Set(Sprite::VI_NW, Vector3(left, y1, .0f));
      fbVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      fbVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      fbVerts.Set(Sprite::VI_NE, Vector3(right, y1, .0f));
    }
  }
}

} // XR
