//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <XR/UIRadioButton.hpp>

namespace XR
{

//==============================================================================
UIRadioButton::List UIRadioButton::s_lButtons;

//==============================================================================
void UIRadioButton::GroupClear(uint32_t groupHash)
{
  for (List::iterator i0(s_lButtons.begin()), i1(s_lButtons.end()); i0 != i1; ++i0)
  {
    UIRadioButton*  p(*i0);
    if (p->m_groupHash == groupHash)
    {
      p->SetSelected(false);
      p->OnSelectedStateChanged();
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
      UIRadioButton*  p(*i0);
      if (p->m_groupHash == groupHash && p != pButton)
      {
        p->SetSelected(false);
        p->OnSelectedStateChanged();
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
void UIRadioButton::SetGroup(const char* pGroupName)
{
  XR_ASSERT(UIRadioButton, pGroupName != 0);
  SetGroup(Hash::String32(pGroupName));
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
bool  UIRadioButton::OnMouseAction(const Input::MouseActionEvent& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  bool  isHandled(false);
  if ((m_pActiveArea == 0 ||
    (e.x >= m_pActiveArea->x &&
    e.y >= m_pActiveArea->y &&
    e.x < m_pActiveArea->x + m_pActiveArea->w &&
    e.y < m_pActiveArea->y + m_pActiveArea->h)) &&
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

} // XR
