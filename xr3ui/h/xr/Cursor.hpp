#ifndef XR_POINTER_HPP
#define XR_POINTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Input.hpp"
#include "UIImage.hpp"

namespace xr
{

//==============================================================================
class Cursor
{
public:
  // structors
  Cursor();
  ~Cursor();

  // general
  void  SetTimeOut(int ms);
  void  SetMoveTreshold(float hPcnt);
  void  SetKeepAliveTreshold(float hPcnt);
  void  SetSprite(const Sprite* sprite);
  void  SetEnabled(bool state);
  void  SetPosition(int x, int y);

  void  Wake();

  void  Update(int ms);
  void  Render(IUIRenderer& r);

private:
  // static
  static void OnMotion(void* systemData, void* userData);
  static void OnAction(void* systemData, void* userData);

  // data
  bool  m_isEnabled;
  UIImage m_image;

  int m_timeOut;
  int m_timer;
  float m_moveTreshold;
  float m_keepAliveTreshold;

  bool  m_isPressed;
};

} // xr

#endif  //XR_POINTER_HPP
