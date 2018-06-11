#ifndef XR_UICHECKBOX_HPP
#define XR_UICHECKBOX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <list>
#include "UIButton.hpp"

namespace xr
{

//==============================================================================
class UICheckBox: public UIButton
{
public:
  // types
  typedef void(*Callback)(UICheckBox* checkBox, void* userData);

  // data
  Callback  onSelectedStateChanged;
  Sprite    setSprite;
  Material::Ptr setMaterial; // TODO: UIBuilder support

  // structors
  UICheckBox();
  ~UICheckBox();

  // general
  bool          IsSelected() const;

  void          SetSelected(bool isIt);

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;

  virtual void  OnSelectedStateChanged();

protected:
  // types
  enum  StateFlags: uint8_t
  {
    SF_SELECTED = SF_PRESSED + 1,
    MASK_SELECTED = XR_MASK_ID(uint8_t, SF_SELECTED)
  };
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  UICheckBox::IsSelected() const
{
  return (m_state & MASK_SELECTED) != 0;
}

} // xr

#endif // XR_UICHECKBOX_HPP