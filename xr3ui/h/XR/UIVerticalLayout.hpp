#ifndef XR_UIVERTICALLAYOUT_HPP
#define XR_UIVERTICALLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIGrowingLayoutBase.hpp"

namespace XR
{

//==============================================================================
class UIVerticalLayout: public UIGrowingLayoutBase
{
public:
  // structors
  UIVerticalLayout();
  ~UIVerticalLayout();

  // general
  virtual int CalculateElementsExtent() const;

protected:
  // internal
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetHeightToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIVerticalLayout::CalculateElementsExtent() const
{
  return m_lElements.empty() ? 0 : (m_growDir == GD_POSITIVE ?
    (m_lElements.back()->CalculateBottom() - m_lElements.front()->y) :
    (m_lElements.front()->CalculateBottom() - m_lElements.back()->y));
}

} // XR

#endif // XR_UIVERTICALLAYOUT_HPP
