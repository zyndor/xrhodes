//
// Nuclear Heart Games
// XRhodes
//
// Cursor.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    25/10/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_POINTER_HPP
#define XR_POINTER_HPP

#include  <XR/Input.hpp>
#include  "UIImage.hpp"

namespace XR
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
  void  SetSprite(const XR::Sprite* pSprite);
  void  SetEnabled(bool state);
  void  SetPosition(int x, int y);
 
  void  Wake();

  void  Update(int ms);
  void  Render();
  void  Render(XR::UIRenderer& r);

private:
  // static
  static void OnMotion(void* pSystem, void* pUser);
  static void OnAction(void* pSystem, void* pUser);

  // data
  bool        m_isEnabled;
  XR::UIImage m_image;

  int         m_timeOut;
  int         m_timer;
  float       m_moveTreshold;
  float       m_keepAliveTreshold;

  bool        m_isPressed;
};

} // XR

#endif  //XR_POINTER_HPP