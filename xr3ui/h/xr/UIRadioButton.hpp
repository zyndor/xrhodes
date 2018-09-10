#ifndef XR_UIRADIOBUTTON_HPP
#define XR_UIRADIOBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <list>
#include "UICheckBox.hpp"

namespace xr
{

//==============================================================================
///@brief A specialised UICheckBox, which can be assigned a group. Selecting a
/// UIRadioButton de-select all other radio buttons in the group.
class UIRadioButton: public UICheckBox
{
public:
  // types
  enum
  {
    NO_GROUP = 0
  };

  // static
  static void GroupRemoveAll(const char* groupName);  // TODO: implement.
  static void GroupRemoveAll(uint32_t groupHash); // TODO: implement.

  // structors
  UIRadioButton();
  ~UIRadioButton();

  // general
  uint32_t      GetGroupHash() const;

  void          SetGroup(const char* groupName);
  void          SetGroup(uint32_t groupHash);

  bool  OnMouseAction(const Input::MouseActionData& e) override;

  void  OnSelectedStateChanged() override;

protected:
  // types
  typedef std::list<UIRadioButton*> List;

  // static
  static List s_lButtons; // no ownership // TODO: Linked<>

  static void GroupClear(uint32_t groupHash);
  static void GroupClearExcept(uint32_t groupHash, const UIRadioButton* button);

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

} // xr

#endif // XR_UIRADIOBUTTON_HPP
