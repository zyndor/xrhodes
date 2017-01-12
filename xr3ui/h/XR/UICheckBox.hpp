//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UICHECKBOX_HPP
#define XR_UICHECKBOX_HPP

#include <list>
#include "UIButton.hpp"

namespace XR
{

//==============================================================================
class UICheckBox: public UIButton
{
public:
  // types
  typedef void(*Callback)(UICheckBox* pCheckBox, void* pData);
  
  // data
  Callback  pOnSelectedStateChanged;
  Sprite    setSprite;
  
  // structors
  UICheckBox();
  ~UICheckBox();
  
  // general
  bool          IsSelected() const;
  
  void          SetSelected(bool isIt);
  
  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

  virtual void  OnSelectedStateChanged();
  
protected:
  // types
  enum  StateFlags
  {
    SF_SELECTED = SF_PRESSED + 1,
    MASK_SELECTED = XR_MASK_ID(SF_SELECTED)
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

} // XR

#endif // XR_UICHECKBOX_HPP