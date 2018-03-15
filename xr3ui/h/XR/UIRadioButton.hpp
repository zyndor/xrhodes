#ifndef XR_UIRADIOBUTTON_HPP
#define XR_UIRADIOBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

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
  static void GroupRemoveAll(uint32_t groupHash);

  // structors
  UIRadioButton();
  ~UIRadioButton();

  // general
  uint32_t      GetGroupHash() const;

  void          SetGroup(const char* pGroupName);
  void          SetGroup(uint32_t groupHash);

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);

  virtual void  OnSelectedStateChanged();

protected:
  // types
  typedef std::list<UIRadioButton*> List;

  // static
  static List s_lButtons; // no ownership

  static void GroupClear(uint32_t groupHash);
  static void GroupClearExcept(uint32_t groupHash, const UIRadioButton* pButton);

  // data
  uint32_t  m_groupHash;
};

//==============================================================================
// implementation
//==============================================================================
inline
uint32_t  UIRadioButton::GetGroupHash() const
{
  return m_groupHash;
}

} // XR

#endif // XR_UIRADIOBUTTON_HPP
