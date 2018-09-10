//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <algorithm>
#include "xr/UIRadioButton.hpp"

namespace xr
{

//==============================================================================
UIRadioButton::List UIRadioButton::s_lButtons;

//==============================================================================
void UIRadioButton::GroupClear(uint32_t groupHash)
{
  for (List::iterator i0(s_lButtons.begin()), i1(s_lButtons.end()); i0 != i1; ++i0)
  {
    UIRadioButton*  button = *i0;
    if (button->m_groupHash == groupHash)
    {
      button->SetSelected(false);
      button->OnSelectedStateChanged();
    }
  }
}

//==============================================================================
void UIRadioButton::GroupClearExcept(uint32_t groupHash, const UIRadioButton* pButton)
{
  if (pButton != 0 && pButton->m_groupHash == groupHash)
  {
    for (List::iterator i0(s_lButtons.begin()), i1(s_lButtons.end()); i0 != i1; ++i0)
    {
      UIRadioButton*  button = *i0;
      if (button->m_groupHash == groupHash && button != pButton)
      {
        button->SetSelected(false);
        button->OnSelectedStateChanged();
      }
    }
  }
}

//==============================================================================
UIRadioButton::UIRadioButton()
: UICheckBox(),
  m_groupHash(NO_GROUP)
{
  s_lButtons.push_back(this);
}

//==============================================================================
UIRadioButton::~UIRadioButton()
{
  List::iterator  iErase(std::find(s_lButtons.begin(), s_lButtons.end(), this));
  s_lButtons.erase(iErase);
}

//==============================================================================
void UIRadioButton::SetGroup(const char* groupName)
{
  XR_ASSERT(UIRadioButton, groupName != nullptr);
  SetGroup(Hash::String32(groupName));
}

//==============================================================================
void UIRadioButton::SetGroup(uint32_t groupHash)
{
  if (m_groupHash != groupHash)
  {
    if (groupHash != NO_GROUP && IsSelected())
    {
      GroupClear(groupHash);
    }

    m_groupHash = groupHash;
  }
}

//==============================================================================
bool  UIRadioButton::OnMouseAction(const Input::MouseActionData& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  bool  isHandled(false);
  if ((!m_activeArea ||
    (e.x >= m_activeArea->x &&
    e.y >= m_activeArea->y &&
    e.x < m_activeArea->x + m_activeArea->w &&
    e.y < m_activeArea->y + m_activeArea->h)) &&
    (e.x >= x && e.x < kRight &&
    e.y >= y && e.y < kBottom))
  {
    if (e.isPressed)
    {
      if (!IsPressed())
      {
        m_state |= MASK_PRESSED;  // press
        m_lastTouch.x = e.x;
        m_lastTouch.y = e.y;
        if (IsEnabled())
        {
          OnPressed();
        }
        isHandled = true;
      }
    }
    else if (IsPressed())
    {
      m_state &= ~MASK_PRESSED;  // release

      if (IsEnabled() &&
        m_lastTouch.x >= x && m_lastTouch.x < kRight &&
        m_lastTouch.y >= y && m_lastTouch.y < kBottom)
      {
        OnReleased();
        SetSelected(true);
        OnSelectedStateChanged();
      }
      isHandled = true;
    }
  }
  else if (IsPressed())
  {
    m_state &= ~MASK_PRESSED; // unpress
  }
  return isHandled;
}

//==============================================================================
void  UIRadioButton::OnSelectedStateChanged()
{
  if (IsSelected() && m_groupHash != NO_GROUP)
  {
    GroupClearExcept(m_groupHash, this);
  }

  UICheckBox::OnSelectedStateChanged();
}

} // xr
