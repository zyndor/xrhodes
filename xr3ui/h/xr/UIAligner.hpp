#ifndef XR_UIALIGNER_HPP
#define XR_UIALIGNER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "UIContainer.hpp"

namespace xr
{

//==============================================================================
///@brief Aligns its children with the given horizontal / vertical alignments,
/// on top of each other.
class UIAligner:  public  UIContainer
{
public:
  // structors
  UIAligner();

  // general
  Alignment GetHorizontalAlignment() const;
  Alignment GetVerticalAlignment() const;

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);

protected:
  // internal use
  void AlignChildImpl( UIElement* elem ) override;

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
