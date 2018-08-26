#ifndef DRAGCONTROLLER_HPP
#define DRAGCONTROLLER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Input.hpp"
#include "xr/Timer.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

// DragController provides the following facilities:
// - Listens to mouse motion and action events;
// - Accumulates the motion of the mouse when the button is pressed;
// - Holding the mouse button fills up a timer (up to 2 seconds), which
// releasing the button depletes, to allow smoothing animation out based on
// this value.
class DragController
{
public:
  // Resets all values (time spent, motion accumulated) to zero.
  void Reset();

  // Attempts to connect to mouse motion and action signals of xr::Input;
  // returns whether either was successful (if the other wasn't, that just
  // means that it was already connected).
  bool ConnectMouse();

  // Attempts to disconnect from mouse motion and action signals of xr::Input;
  // returns whether either was successful (if the other wasn't, that just
  // means that it was already connected).
  bool DisconnectMouse();

  // Updates the timers, adds the motion accumulated during this frame
  // (i.e. since the last call to Update()) to the total.
  void Update();

  // Gives a percentage value (in the range of 0. to 1.) of how long the
  // left button was pressed, munus how long ago was it released, based
  // on a 2 seconds maxium.
  float GetPressTimerPercent() const;

  // Gives the magnitude and direction of the movement accumulated during
  // the last frame (which may be used as a velocity value).
  xr::Vector2 const& GetFrameMotion() const;

  // Gives the magnitude and direction of the total movement since the
  // last call to Reset().
  xr::Vector2 const& GetTotalMotion() const;

private:
  static const int64_t kMaxPressDurationMs = 2000;

  bool m_isButtonDown = false; // is the button pressed and held?
  uint64_t m_tLastFrameMs = 0;  // timestamp of last frame
  int64_t m_pressTimerMs = 0; // amount of time button was pressed.

  xr::SVector2 m_lastMousePosition; // last position of mouse in screen space
  xr::Vector2 m_accumulators[2];
  int m_iActiveAccumulator = 0;
  xr::Vector2 m_motion;

  int NextAccumulator() const;

  void OnMouseAction(const xr::Input::MouseActionData& e);
  void OnMouseMotion(const xr::Input::MouseMotionData& e);
};

#endif //
