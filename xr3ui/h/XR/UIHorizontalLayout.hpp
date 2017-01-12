//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIHORIZONTALLAYOUT_HPP
#define XR_UIHORIZONTALLAYOUT_HPP

#include "UIGrowingLayoutBase.hpp"

namespace XR
{

//==============================================================================
///@brief container that places the elements within side-by-side
class UIHorizontalLayout: public UIGrowingLayoutBase
{
public:
  // structors
  UIHorizontalLayout();
  virtual ~UIHorizontalLayout();

  // general
  virtual int CalculateElementsExtent() const;

protected:
  // internal
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetWidthToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIHorizontalLayout::CalculateElementsExtent() const
{
  return m_lElements.empty() ? 0 : (m_growDir == GD_POSITIVE ? 
    (m_lElements.back()->CalculateRight() - m_lElements.front()->x) :
    (m_lElements.front()->CalculateRight() - m_lElements.back()->x));
}

} // XR

#endif // XR_UIHORIZONTALLAYOUT_HPP
