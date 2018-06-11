#ifndef XR_UIALIGNER_HPP
#define XR_UIALIGNER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIContainer.hpp"

namespace xr
{

//==============================================================================
class UIAligner:  public  UIContainer
{
public:
  // structors
  UIAligner();
  virtual ~UIAligner();

  // general
  Alignment GetHorizontalAlignment() const;
  Alignment GetVerticalAlignment() const;

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);

protected:
  // internal use
  virtual void _AlignElement( UIElement* elem );

  // data
  Alignment m_hAlign;
  Alignment m_vAlign;

};

//==============================================================================
// implementation
//==============================================================================
inline
UIElement::Alignment UIAligner::GetHorizontalAlignment() const
{
  return m_hAlign;
}

//==============================================================================
inline
UIElement::Alignment UIAligner::GetVerticalAlignment() const
{
  return m_vAlign;
}

} // xr

#endif // XR_UIALIGNER_HPP