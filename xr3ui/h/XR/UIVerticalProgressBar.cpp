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
    (!isFdPositive && m_percentage < percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    float y1(h * perc);

    float ws(w / (sprite.GetHalfWidth() * 2.0f));
    float left(x + sprite.GetLeftPadding() * ws);
    float right(x + w - sprite.GetRightPadding() * ws);

    const RenderStream& rsSpriteUVs(sprite.GetUVs());
    float uv1;
    if (sprite.IsUVRotated())
    {
      uv1 = (percMax - perc) * (rsSpriteUVs.GetX(Sprite::VI_NW) -
        rsSpriteUVs.GetX(Sprite::VI_SW)) / sprite.GetQuadHeight() *
        hSprite;
    }
    else
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetY(Sprite::VI_SW) -
        rsSpriteUVs.GetY(Sprite::VI_NW)) / sprite.GetQuadHeight() *
        hSprite;
    }

    RenderStream* pRsVerts(Renderer::AllocStream(RenderStream::F_VECTOR3,
      Sprite::kNumVertices));
    RenderStream* pRsUVs(Renderer::AllocStream(RenderStream::F_VECTOR2,
      Sprite::kNumVertices));
    sprite.CopyUVsTo(0, *pRsUVs);

    y1 += y;

    if (isFdPositive)
    {
      int16 top(y + yMin);

      if (sprite.IsUVRotated())
      {
        pRsUVs->SetX(Sprite::VI_SW, uv1);
        pRsUVs->SetX(Sprite::VI_SE, uv1);
      }
      else
      {
        pRsUVs->SetY(Sprite::VI_SW, uv1);
        pRsUVs->SetY(Sprite::VI_SE, uv1);
      }

      pRsVerts->Set(Sprite::VI_NW, Vector3(left, top, .0f));
      pRsVerts->Set(Sprite::VI_SW, Vector3(left, y1, .0f));
      pRsVerts->Set(Sprite::VI_SE, Vector3(right, y1, .0f));
      pRsVerts->Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
    else
    {
      int16 bottom(y + yMax);

      if (sprite.IsUVRotated())
      {
        pRsUVs->SetX(Sprite::VI_SW, uv1);
        pRsUVs->SetX(Sprite::VI_SE, uv1);
      }
      else
      {
        pRsUVs->SetY(Sprite::VI_NW, uv1);
        pRsUVs->SetY(Sprite::VI_NE, uv1);
      }

      pRsVerts->Set(Sprite::VI_NW, Vector3(left, y1, .0f));
      pRsVerts->Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      pRsVerts->Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      pRsVerts->Set(Sprite::VI_NE, Vector3(right, y1, .0f));
    }

    Renderer::SetMaterial(sprite.GetMaterial());
    Renderer::SetAmbientColor(color);

    Renderer::SetVertStream(*pRsVerts);
    Renderer::SetUVStream(*pRsUVs);

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
    (!isFdPositive && m_percentage < percMax))
  {
    float perc(Clamp(m_percentage, percMin, percMax));
    float y1(h * perc);

    float ws(w / (sprite.GetHalfWidth() * 2.0f));
    float left(x + sprite.GetLeftPadding() * ws);
    float right(x + w - sprite.GetRightPadding() * ws);

    const RenderStream&  rsSpriteUVs(sprite.GetUVs());
    float uv1;
    if (sprite.IsUVRotated())
    {
      uv1 = (percMax - perc) * (rsSpriteUVs.GetX(Sprite::VI_NW) -
        rsSpriteUVs.GetX(Sprite::VI_SW)) / sprite.GetQuadHeight() *
        hSprite;
    }
    else
    {
      uv1 = (perc - percMin) * (rsSpriteUVs.GetY(Sprite::VI_SW) -
        rsSpriteUVs.GetY(Sprite::VI_NW)) / sprite.GetQuadHeight() *
        hSprite;
    }

    RenderStream  rsUVs;
    RenderStream  rsVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, rsUVs));
    sprite.CopyUVsTo(0, rsUVs);

    y1 += y;

    if (isFdPositive)
    {
      float top(y + yMin);

      if (sprite.IsUVRotated())
      {
        rsUVs.SetX(Sprite::VI_SW, uv1);
        rsUVs.SetX(Sprite::VI_SE, uv1);
      }
      else
      {
        rsUVs.SetY(Sprite::VI_SW, uv1);
        rsUVs.SetY(Sprite::VI_SE, uv1);
      }

      rsVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f));
      rsVerts.Set(Sprite::VI_SW, Vector3(left, y1, .0f));
      rsVerts.Set(Sprite::VI_SE, Vector3(right, y1, .0f));
      rsVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
    }
    else
    {
      float bottom(y + yMax);

      if (sprite.IsUVRotated())
      {
        rsUVs.SetX(Sprite::VI_SW, uv1);
        rsUVs.SetX(Sprite::VI_SE, uv1);
      }
      else
      {
        rsUVs.SetY(Sprite::VI_NW, uv1);
        rsUVs.SetY(Sprite::VI_NE, uv1);
      }

      rsVerts.Set(Sprite::VI_NW, Vector3(left, y1, .0f));
      rsVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
      rsVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
      rsVerts.Set(Sprite::VI_NE, Vector3(right, y1, .0f));
    }
  }
}

} // XR
