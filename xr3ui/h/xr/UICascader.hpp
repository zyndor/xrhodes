#ifndef XR_UICASCADER_HPP
#define XR_UICASCADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIAligner.hpp"

namespace xr
{

//==============================================================================
///@brief Adds a horizontal and vertical offset to each subsequent child.
class UICascader: public UIAligner
{
public:
  // structors
  UICascader();

  // general use
  int GetHorizontalOffset() const;
  int GetVerticalOffset() const;

  void  SetHorizontalOffset(int val);
  void  SetVerticalOffset(int val);

protected:
  // internal use
  void AlignChildImpl( UIElement* elem ) override;

  void SetWidthToContentImpl() override;
  void SetHeightToContentImpl() override;

  // data
  int m_horizontalOffset;
  int m_verticalOffset;
};

//==============================================================================
//  implementation
//==============================================================================
inline
int UICascader::GetHorizontalOffset() const
{
  return m_horizontalOffset;
}

//==============================================================================
inline
int UICascader::GetVerticalOffset() const
{
  return m_verticalOffset;
}

} // xr

#endif // XR_UICASCADER_HPP
