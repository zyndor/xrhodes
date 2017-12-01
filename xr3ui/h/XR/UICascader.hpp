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

namespace XR
{

//==============================================================================
class UICascader: public UIAligner
{
public:
  // structors
  UICascader();
  virtual ~UICascader();

  // general use
  int GetHorizontalOffset() const;
  int GetVerticalOffset() const;

  void  SetHorizontalOffset(int val);
  void  SetVerticalOffset(int val);

protected:
  // internal use
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetWidthToContent();
  virtual void _SetHeightToContent();

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

} // XR

#endif // XR_UICASCADER_HPP