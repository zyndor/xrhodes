#ifndef XR_UIBUTTON_HPP
#define XR_UIBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIColoredElement.hpp"
#include "XR/utils.hpp"
#include "XR/Sprite.hpp"

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

  typedef void(*Callback)(UIButton *, void *data);

  // data
  Sprite    sprites[kNumStates];
  Material::Ptr materials[kNumStates]; // TODO: UIBuilder support
  Callback  onPressed;
  Callback  onReleased;
  void*     callbackData;

  // structors
  UIButton();
  virtual ~UIButton();

  // general
  bool  IsEnabled() const;
  bool  IsPressed() const;

  void  SetEnabled(bool isIt);
  void  SetPressed(bool isIt);

  int   GetSpriteId() const;

  void  SetSprites(const Sprite* sprite, float scale);
  void  SetSprites(const Sprite* sprites_[kNumStates], float scale);
  void  SetSizeToSprite(float scale);

  void  SetActiveArea(const Rect* activeArea); // no ownership transfer

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;

  void  OnPressed();
  void  OnReleased();

protected:
  // types
  enum  StateFlags: uint8_t
  {
    SF_ENABLED,
    SF_PRESSED,
    MASK_ENABLED = XR_MASK_ID(uint8_t, SF_ENABLED),
    MASK_PRESSED = XR_MASK_ID(uint8_t, SF_PRESSED)
  };

  // data
  const Rect*  m_activeArea; // no ownership

  uint8_t m_state;

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
