#ifndef XR_UIHORIZONTALLAYOUT_HPP
#define XR_UIHORIZONTALLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIGrowingLayoutBase.hpp"

namespace xr
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
  virtual void _AlignElement( UIElement* elem );

  virtual void _SetWidthToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIHorizontalLayout::CalculateElementsExtent() const
{
  return m_elements.empty() ? 0 : (m_growDir == GD_POSITIVE ?
    (m_elements.back()->CalculateRight() - m_elements.front()->x) :
    (m_elements.front()->CalculateRight() - m_elements.back()->x));
}

} // xr

#endif // XR_UIHORIZONTALLAYOUT_HPP
