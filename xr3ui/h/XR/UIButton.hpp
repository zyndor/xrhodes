//
// Nuclear Heart Games
// XRhodes
//
// UIButton.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIBUTTON_HPP
#define XR_UIBUTTON_HPP

#include <XR/utils.hpp>
#include <XR/Sprite.hpp>
#include "UIColoredElement.hpp"

namespace XR
{

//==============================================================================
///@brief A button.
class UIButton:  public UIColoredElement
{
public:
  // types
  enum  State
  {
    S_UP,
    S_DOWN,
    S_OFF,
    kNumStates
  };

  typedef void(*Callback)(UIButton *, void *pData);

  // data
  Callback  pOnPressed;
  Callback  pOnReleased;
  void*     pCallbackData;
  Sprite    arSprite[kNumStates];

  // structors
  UIButton();
  virtual ~UIButton();

  // general
  bool  IsEnabled() const;
  bool  IsPressed() const;

  void  SetEnabled(bool isIt);
  void  SetPressed(bool isIt);

  int   GetSpriteId() const;

  void  SetSprites(const Sprite* pSprite, float scale);
  void  SetSprites(const Sprite* arpSprite[kNumStates], float scale);
  void  SetSizeToSprite(float scale);

  void  SetActiveArea(const Rect* pActiveArea); // no ownership transfer

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

  void  OnPressed();
  void  OnReleased();

protected:
  // types
  enum  StateFlags
  {
    SF_ENABLED,
    SF_PRESSED,
    MASK_ENABLED = XR_MASK_ID(SF_ENABLED),
    MASK_PRESSED = XR_MASK_ID(SF_PRESSED)
  };

  // data
  const Rect*  m_pActiveArea; // no ownership

  uint8 m_state;
  
  SVector2  m_lastTouch;
};

//==============================================================================
//  implementation
//==============================================================================
inline
bool UIButton::IsEnabled() const
{
  return (m_state & MASK_ENABLED) != 0;
}

//==============================================================================
inline
bool UIButton::IsPressed() const
{
  return (m_state & MASK_PRESSED) != 0;
}

} // XR

#endif // XR_UIBUTTON_HPP
