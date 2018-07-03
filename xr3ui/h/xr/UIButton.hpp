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
#include "xr/utils.hpp"
#include "xr/Sprite.hpp"

namespace xr
{

//==============================================================================
///@brief A button which have three possible states, each represented by a
/// Sprite. 
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

  using Callback = void(*)(UIButton*, void *data);

  // data
  Sprite    sprites[kNumStates];
  Material::Ptr materials[kNumStates]; // TODO: UIBuilder support
  Callback  onPressed;
  Callback  onReleased;
  void*     callbackData;

  // structors
  UIButton();

  // general
  bool  IsEnabled() const;
  bool  IsPressed() const;

  void  SetEnabled(bool isIt);
  void  SetPressed(bool isIt);

  ///@return The index into sprites, based on state.
  int   GetSpriteId() const;

  ///@brief Sets all three state Sprites to @a sprite at @a scale.
  void  SetSprites(const Sprite* sprite, float scale);

  ///@brief Sets the state Sprites to each corresponding entry in @a sprites,
  /// at @a scale.
  void  SetSprites(const Sprite* sprites_[kNumStates], float scale);

  ///@brief Sets the dimensions of the button to that of the first state Sprite,
  /// at @a scale.
  void  SetSizeToSprite(float scale);

  ///@brief Sets, or if nullptr, removes an @a activeArea, which, if present,
  /// is first checked before determining whether the button was hit.
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

} // xr

#endif // XR_UIBUTTON_HPP
