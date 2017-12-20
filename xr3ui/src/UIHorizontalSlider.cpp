//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIHorizontalSlider.hpp"
#include "XR/UIRenderer.hpp"
#include "XR/utils.hpp"

namespace XR
{

//==============================================================================
UIHorizontalSlider::UIHorizontalSlider()
: UISliderBase()
{}

//==============================================================================
UIHorizontalSlider::~UIHorizontalSlider()
{}

//==============================================================================
int UIHorizontalSlider::CalculateRange() const
{
  int ssw(GetSliderSpriteWidth());
  int range(w - ssw);
  XR_ASSERTMSG(UIHorizontalSlider, range > 0,
    ("Slider width (%d) - slider's sprite's width (%d) needs to be > 0.",
    w, ssw));
  return range;
}

//==============================================================================
bool UIHorizontalSlider::OnMouseAction(const Input::MouseActionEvent& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  int xTouch(e.x);
  if (e.isPressed && e.y >= y && e.y < kBottom &&
    xTouch >= x && xTouch < kRight)
  {
    // calculate which side we're on
    xTouch -= x;

    int value(CalculateValue());
    if (xTouch < value)
    {
      xTouch -= sliderSprite.GetHalfWidth();
      SetValue(xTouch);
    }
    else
    {
      xTouch -= GetSliderSpriteWidth();

      if (xTouch > value)
      {
        xTouch += sliderSprite.GetHalfWidth();
        SetValue(xTouch);
      }
      else
      {
        m_isTouched = true;
        m_touchPosition = e.x;
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
bool UIHorizontalSlider::OnMouseMotion(const Input::MouseMotionEvent& e)
{
  if (m_isTouched)
  {
    int dx(e.x - m_touchPosition);
    m_touchPosition = e.x;
    SetValue(CalculateValue() + dx);
  }
  return false;
}

//==============================================================================
void UIHorizontalSlider::Render(IUIRenderer& renderer) const
{
  // render the rail
  if (sprite.GetMaterial())
  {
    UIImage::Render(renderer);
  }

  // render the slider
  XR_ASSERTMSG(UIHorizontalSlider, sliderSprite.GetMaterial() != nullptr,
    ("Material needs to be set in sliderSprite before Render()"));

  int16_t left(x + CalculateValue() + sliderSprite.GetLeftPadding());
  int16_t top(y + h / 2 - Round(sliderSprite.GetHalfHeight() -
    sliderSprite.GetTopPadding()));
  int16_t right(left + sliderSprite.GetQuadWidth());
  int16_t bottom(top + sliderSprite.GetQuadHeight());

  auto verts = renderer.NewSprite(sliderSprite.GetMaterial());
  verts[Sprite::VI_NW].pos = Vector3(left, top, .0f);
  verts[Sprite::VI_SW].pos = Vector3(left, bottom, .0f);
  verts[Sprite::VI_SE].pos = Vector3(right, bottom, .0f);
  verts[Sprite::VI_NE].pos = Vector3(right, top, .0f);

  sliderSprite.CopyUVsTo(verts);

  _ApplyColor(verts);
}

} // XR
