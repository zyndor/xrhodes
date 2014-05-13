//
// Nuclear Heart Games
// XRhodes
//
// UIRadioButton.hpp
// @author  Gyorgy Straub
// @date    23/08/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIRADIOBUTTON_HPP
#define XR_UIRADIOBUTTON_HPP

#include <list>
#include "UICheckBox.hpp"

namespace XR
{

//==============================================================================
class UIRadioButton: public UICheckBox
{
public:
  // types
  enum
  {
    NO_GROUP = 0
  };

  // static
  static void GroupRemoveAll(const char* pGroupName);
  static void GroupRemoveAll(uint32 groupHash);

  // structors
  UIRadioButton();
  ~UIRadioButton();
  
  // general
  uint32        GetGroupHash() const;

  void          SetGroup(const char* pGroupName);
  void          SetGroup(uint32 groupHash);

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);

  virtual void  OnSelectedStateChanged();
  
protected:
  // types
  typedef std::list<UIRadioButton*> List;

  // static
  static List s_lButtons; // no ownership

  static void GroupClear(uint32 groupHash);
  static void GroupClearExcept(uint32 groupHash, const UIRadioButton* pButton);

  // data
  uint32  m_groupHash;
};

//==============================================================================
// implementation
//==============================================================================
inline
uint32  UIRadioButton::GetGroupHash() const
{
  return m_groupHash;
}

} // XR

#endif // XR_UIRADIOBUTTON_HPP