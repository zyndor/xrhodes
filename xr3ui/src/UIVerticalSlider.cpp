//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIVerticalSlider.hpp"
#include "xr/UIRenderer.hpp"
#include "xr/utils.hpp"

namespace xr
{

//==============================================================================
UIVerticalSlider::UIVerticalSlider()
: UISliderBase()
{}

//==============================================================================
int32_t UIVerticalSlider::CalculateRange() const
{
  int32_t ssh = GetSliderSpriteHeight();
  int32_t range = h - ssh;
  XR_ASSERTMSG(UIVerticalSlider, range > 0,
    ("Slider height (%d) - slider's sprite's height (%d) needs to be > 0.",
    h, ssh));
  return range;
}

//==============================================================================
bool UIVerticalSlider::OnMouseAction(const Input::MouseActionData& e )
{
  const int32_t kRight = CalculateRight();
  const int32_t kBottom = CalculateBottom();

  int yTouch(e.y);
  if (e.isPressed && e.x >= x && e.x < kRight &&
    yTouch >= y && yTouch < kBottom)
  {
    // calculate which side we're on
    yTouch -= y;

    int32_t value = CalculateValue();
    if (yTouch < value)
    {
      yTouch -= static_cast<int32_t>(sliderSprite.GetHalfHeight());
      SetValue(yTouch);
    }
    else
    {
      yTouch -= GetSliderSpriteHeight();

      if (yTouch > value)
      {
        yTouch += static_cast<int32_t>(sliderSprite.GetHalfHeight());
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
bool UIVerticalSlider::OnMouseMotion(const Input::MouseMotionData& e )
{
  if (m_isTouched)
  {
    int32_t dy = e.y - m_touchPosition;
    m_touchPosition = e.y;
    SetValue(CalculateValue() + dy);
  }
  return false;
}

//==============================================================================
void UIVerticalSlider::Render(IUIRenderer& renderer) const
{
  // render the rail
  if (material)
  {
    UIImage::Render(renderer);
  }

  // render the slider
  XR_ASSERTMSG(UIVerticalSlider, sliderMaterial != nullptr,
    ("Material needs to be set in sliderSprite before Render()"));

  float left = x + w / 2 - sliderSprite.GetHalfWidth() + sliderSprite.GetLeftPadding();
  float top = y + CalculateValue() + sliderSprite.GetTopPadding();
  float right = left + sliderSprite.GetQuadWidth();
  float bottom = top + sliderSprite.GetQuadHeight();

  auto verts = renderer.NewSprite(sliderMaterial);
  verts[Quad::Vertex::NW].pos = Vector3(left, top, .0f);
  verts[Quad::Vertex::NE].pos = Vector3(right, top, .0f);
  verts[Quad::Vertex::SW].pos = Vector3(left, bottom, .0f);
  verts[Quad::Vertex::SE].pos = Vector3(right, bottom, .0f);

  sliderSprite.CopyUVsTo(verts);

  ApplyColor(verts);
}

} // xr
