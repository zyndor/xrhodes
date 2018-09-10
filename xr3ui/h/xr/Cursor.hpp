#ifndef XR_POINTER_HPP
#define XR_POINTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Input.hpp"
#include "UIImage.hpp"

namespace xr
{

//==============================================================================
///@brief Cursor positions the top left of an image where the mouse pointer is,
/// goes to sleep (fading out) following a period of inactivity, and wakes up
/// (going to maximum alpha) when the mouse motion or button press occurs.
class Cursor
{
public:
  // structors
  Cursor();

  // general
  void  SetTimeOut(int ms);

  ///@brief Sets the movement threshold for waking a sleeping Cursor, in terms
  /// of a percentage of the screen height.
  ///@note The use of a greater value than for keep alive, is recommended.
  void  SetMoveTreshold(float hPcnt);

  ///@brief Sets the threshold of motion for keeping the Cursor awake, in terms
  /// of a percentage of the screen height.
  ///@note The use of a smaller value than for move, is recommended.
  void  SetKeepAliveTreshold(float hPcnt);

  ///@brief Sets the sprite used for the Cursor.
  void  SetSprite(const Sprite* sprite);

  ///@brief Sets whether the Cursor listens to mouse action / motion events,
  /// and is rendered.
  void  SetEnabled(bool state);

  ///@brief Sets the position of the top left of the cursor.
  void  SetPosition(int x, int y);

  ///@brief Resets the timer and ensures that the Cursor is visible (unless disabled).
  void  Wake();

  ///@brief Updates the timer and handles fadeout.
  void  Update(int ms);

  void  Render(IUIRenderer& r);

private:
  // data
  bool  m_isEnabled;
  UIImage m_image;

  int m_timeOut;
  int m_timer;
  float m_moveTreshold;
  float m_keepAliveTreshold;

  bool  m_isPressed;

  // internal
  void OnMotion(Input::MouseMotionData const& e);
  void OnAction(Input::MouseActionData const& e);
};

} // xr

#endif  //XR_POINTER_HPP
