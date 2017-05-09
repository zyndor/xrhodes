//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/UIVerticalSlider.hpp>
#include <XR/UIRenderer.hpp>
#include <XR/utils.hpp>

namespace XR
{

//==============================================================================
UIVerticalSlider::UIVerticalSlider()
: UISliderBase()
{}

//==============================================================================
UIVerticalSlider::~UIVerticalSlider()
{}

//==============================================================================
int UIVerticalSlider::CalculateRange() const
{
  int ssh(GetSliderSpriteHeight());
  int range(h - ssh);
  XR_ASSERTMSG(UIVerticalSlider, range > 0,
    ("Slider height (%d) - slider's sprite's height (%d) needs to be > 0.",
    h, ssh));
  return range;
}

//==============================================================================
bool UIVerticalSlider::OnMouseAction(const Input::MouseActionEvent& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  int yTouch(e.y);
  if (e.isPressed && e.x >= x && e.x < kRight &&
    yTouch >= y && yTouch < kBottom)
  {
    // calculate which side we're on
    yTouch -= y;

    int value(CalculateValue());
    if (yTouch < value)
    {
      yTouch -= sliderSprite.GetHalfHeight();
      SetValue(yTouch);
    }
    else
    {
      yTouch -= GetSliderSpriteHeight();

      if (yTouch > value)
      {
        yTouch += sliderSprite.GetHalfHeight();
        SetValue(yTouch);
      }
      else
      {
        m_isTouched = true;
        m_touchPosition = e.y;
      }
    }
  }
  else if (!e.isPressed)
  {
    m_isTouched = false;
  }

  return false;
}

//==============================================================================
bool UIVerticalSlider::OnMouseMotion(const Input::MouseMotionEvent& e )
{
  if (m_isTouched)
  {
    int dy(e.y - m_touchPosition);
    m_touchPosition = e.y;
    SetValue(CalculateValue() + dy);
  }
  return false;
}

//==============================================================================
void UIVerticalSlider::Render() const
{
  // render the rail
  if (sprite.GetMaterial() != 0)
  {
    UIImage::Render();
  }

  // render the slider
  XR_ASSERTMSG(UIVerticalSlider, sliderSprite.GetMaterial() != 0,
    ("Material needs to be set in sliderSprite before Render()"));

  int left(x + w / 2 - Round(sliderSprite.GetHalfWidth() +
    sliderSprite.GetLeftPadding()));
  int top(y + CalculateValue() + sliderSprite.GetTopPadding());
  int right(left + sliderSprite.GetQuadWidth());
  int bottom(top + sliderSprite.GetQuadHeight());

  FloatBuffer* pFbVerts(Renderer::AllocBuffer(sizeof(Vector3),
    Sprite::kNumVertices));
  pFbVerts->Set(Sprite::VI_NW, Vector3(left, top, .0f));
  pFbVerts->Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
  pFbVerts->Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
  pFbVerts->Set(Sprite::VI_NE, Vector3(right, top, .0f));

  if (sliderSprite.GetMaterial() != sprite.GetMaterial())
  {
    Renderer::SetMaterial(sliderSprite.GetMaterial());
  }

  Renderer::SetAmbientColor(color);

  Renderer::SetUVStream(*sliderSprite.CopyUVs());
  Renderer::SetVertStream(*pFbVerts);
  Renderer::SetColStream();
  Renderer::DrawPrims(PrimType::TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
}

//==============================================================================
void UIVerticalSlider::Render( UIRenderer* pRenderer ) const
{
  // render the rail
  if (sprite.GetMaterial() != 0)
  {
    UIImage::Render(pRenderer);
  }

  // render the slider
  XR_ASSERTMSG(UIVerticalSlider, sliderSprite.GetMaterial() != 0,
    ("Material needs to be set in sliderSprite before Render()"));

  int left(x + w / 2 - Round(sliderSprite.GetHalfWidth() +
    sliderSprite.GetLeftPadding()));
  int top(y + CalculateValue() + sliderSprite.GetTopPadding());
  int right(left + sliderSprite.GetQuadWidth());
  int bottom(top + sliderSprite.GetQuadHeight());

  FloatBuffer  fbVerts(pRenderer->NewSprite(sliderSprite.GetMaterial(),
    sliderSprite.GetUVs(), color));
  fbVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f));
  fbVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f));
  fbVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f));
  fbVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
}

} // XR
