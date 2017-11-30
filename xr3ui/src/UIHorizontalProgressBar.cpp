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

    const FloatBuffer& fbSpriteUVs(sprite.GetUVs());
    const float wRatio(wSprite / sprite.GetQuadWidth());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * wRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_SE).y, uv1);
    }
    else
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_NE).x, uv1);
    }

    FloatBuffer* pfbVerts(Renderer::AllocBuffer(sizeof(Vector3),
                    Sprite::kNumVertices));
    FloatBuffer* pfbUVs(Renderer::AllocBuffer(sizeof(Vector2),
                    Sprite::kNumVertices));
    sprite.CopyUVsTo(0, *pfbUVs);

    x1 += x;

    if (isFdPositive)
    {
      float left(x + xMin);

      if (sprite.IsUVRotated())
      {
        pfbUVs->Get<Vector2>(Sprite::VI_SE).y = uv1;
        pfbUVs->Get<Vector2>(Sprite::VI_NE).y = uv1;
      }
      else
      {
        pfbUVs->Get<Vector2>(Sprite::VI_SE).x = uv1;
        pfbUVs->Get<Vector2>(Sprite::VI_NE).x = uv1;
      }

      pfbVerts->Set(Sprite::VI_NW, Vector3(left, top, .0f));
      pfbVerts->Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      pfbVerts->Set(Sprite::VI_SE, Vector3(x1, bottom, .0f));
      pfbVerts->Set(Sprite::VI_NE, Vector3(x1, top, .0f));
    }
    else
    {
      float right(x + xMax);

      if (sprite.IsUVRotated())
      {
        pfbUVs->Get<Vector2>(Sprite::VI_NW).x = uv1;
        pfbUVs->Get<Vector2>(Sprite::VI_SW).x = uv1;
      }
      else
      {
        pfbUVs->Get<Vector2>(Sprite::VI_NW).x = uv1;
        pfbUVs->Get<Vector2>(Sprite::VI_SW).x = uv1;
      }

      pfbVerts->Set(Sprite::VI_NW, Vector3(x1, top, .0f));
      pfbVerts->Set(Sprite::VI_SW, Vector3(x1, bottom, .0f));
      pfbVerts->Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      pfbVerts->Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }

    sprite.GetMaterial()->Apply();
    Renderer::SetAmbientColor(color);

    Renderer::SetVertStream(*pfbVerts);
    Renderer::SetUVStream(*pfbUVs);

    Renderer::DrawPrims(PrimType::TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
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

    const FloatBuffer& fbSpriteUVs(sprite.GetUVs());
    const float wRatio(wSprite / sprite.GetQuadWidth());
    float uv1((isFdPositive ? (perc - percMin) : (percMax - perc)) * wRatio);
    if (sprite.IsUVRotated())
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_SE).y, uv1);
    }
    else
    {
      uv1 = Lerp(fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x,
        fbSpriteUVs.Get<Vector2>(Sprite::VI_NE).x, uv1);
    }

    FloatBuffer  fbUVs;
    FloatBuffer  fbVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, fbUVs));
    sprite.CopyUVsTo(0, fbUVs);

    x1 += x;

    if (isFdPositive)
    {
      float left(x + xMin);

      if (sprite.IsUVRotated())
      {
        fbUVs.Get<Vector2>(Sprite::VI_SE).y = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_NE).y = uv1;
      }
      else
      {
        fbUVs.Get<Vector2>(Sprite::VI_SE).x = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_NE).x = uv1;
      }

      fbVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f));
      fbVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      fbVerts.Set(Sprite::VI_SE, Vector3(x1, bottom, .0f));
      fbVerts.Set(Sprite::VI_NE, Vector3(x1, top, .0f));
    }
    else
    {
      float right(x + xMax);

      if (sprite.IsUVRotated())
      {
        fbUVs.Get<Vector2>(Sprite::VI_NW).y = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_SW).y = uv1;
      }
      else
      {
        fbUVs.Get<Vector2>(Sprite::VI_NW).x = uv1;
        fbUVs.Get<Vector2>(Sprite::VI_SW).x = uv1;
      }

      fbVerts.Set(Sprite::VI_NW, Vector3(x1, top, .0f));
      fbVerts.Set(Sprite::VI_SW, Vector3(x1, bottom, .0f));
      fbVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      fbVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
  }
}

} // XR
