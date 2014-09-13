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

  int16 xMin(Round(sprite.GetLeftPadding() * ws));
  int16 xMax(w - Round(sprite.GetRightPadding() * ws));
  
  float percMin(static_cast<float>(xMin) / w); 
  float percMax(static_cast<float>(xMax) / w);

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage < percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    int16 x1(Round(w * perc));

    float hs(h / (sprite.GetHalfHeight() * 2.0f));
    int16 top(y + Round(sprite.GetTopPadding() * hs));
    int16 bottom(y + h - Round(sprite.GetBottomPadding() * hs));

    const RenderStream& rsSpriteUVs(sprite.GetUVs());
    float uv1;
    if (sprite.IsUVRotated())
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetVector2(Sprite::VI_SE).y -
        rsSpriteUVs.GetVector2(Sprite::VI_SW).y) / sprite.GetQuadWidth() *
        wSprite;
    }
    else
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetVector2(Sprite::VI_NE).x -
        rsSpriteUVs.GetVector2(Sprite::VI_NW).x) / sprite.GetQuadWidth() *
        wSprite;
    }

    RenderStream* pRsVerts(Renderer::AllocStream(RenderStream::F_VECTOR3,
                    Sprite::kNumVertices));
    RenderStream* pRsUVs(Renderer::AllocStream(RenderStream::F_VECTOR2,
                    Sprite::kNumVertices));
    sprite.CopyUVsTo(0, *pRsUVs);

    x1 += x;

    if (isFdPositive)
    {
      int16 left(x + xMin);

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
      int16 right(x + xMax);

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

  int16 xMin(Round(sprite.GetLeftPadding() * ws));
  int16 xMax(w - Round(sprite.GetRightPadding() * ws));

  float percMin(static_cast<float>(xMin) / w); 
  float percMax(static_cast<float>(xMax) / w);

  if ((isFdPositive && m_percentage >= percMin) ||
    (!isFdPositive && m_percentage < percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    int16 x1(Round(w * perc));

    float hs(h / (sprite.GetHalfHeight() * 2.0f));
    float top(y + Round(sprite.GetTopPadding() * hs));
    float bottom(y + h - Round(sprite.GetBottomPadding() * hs));

    const RenderStream& rsSpriteUVs(sprite.GetUVs());
    float uv1;
    if (sprite.IsUVRotated())
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetY(Sprite::VI_SE) -
        rsSpriteUVs.GetY(Sprite::VI_SW)) / sprite.GetQuadWidth() *
        wSprite;
    }
    else
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetX(Sprite::VI_NE) -
        rsSpriteUVs.GetX(Sprite::VI_NW)) / sprite.GetQuadWidth() *
        wSprite;
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

      rsVerts.Set(Sprite::VI_NW, Vector2(x1, top));
      rsVerts.Set(Sprite::VI_SW, Vector2(x1, bottom));
      rsVerts.Set(Sprite::VI_SE, Vector2(right, bottom));
      rsVerts.Set(Sprite::VI_NE, Vector2(right, top));
    }
  }
}

} // XR
