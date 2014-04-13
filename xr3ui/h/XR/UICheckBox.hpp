//
// Nuclear Heart Games
// XRhodes
//
// UICheckBox.hpp
// @author  Gyorgy Straub
// @date    23/08/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
#if !defined XR_UICHECKBOX_HPP
#define XR_UICHECKBOX_HPP

#include <list>
#include "UIButton.hpp"

namespace XR
{

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

/*
  implementation
*/


} // XR

#endif // XR_UICHECKBOX_HPP