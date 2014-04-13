//
// Nuclear Heart Games
// XRhodes
//
// UICascader.hpp
// @author  Gyorgy Straub
// @date    25/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UICASCADER_HPP
#define XR_UICASCADER_HPP

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
  int16 GetHorizontalOffset() const;
  int16 GetVerticalOffset() const;

  void  SetHorizontalOffset(int16 val);
  void  SetVerticalOffset(int16 val);

protected:
  // internal use
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetWidthToContent();
  virtual void _SetHeightToContent();

  // data
  int16 m_horizontalOffset;
  int16 m_verticalOffset;
};

//==============================================================================
//  implementation
//==============================================================================
inline
int16 UICascader::GetHorizontalOffset() const
{
  return m_horizontalOffset;
}

//==============================================================================
inline
int16 UICascader::GetVerticalOffset() const
{
  return m_verticalOffset;
}

} // XR

#endif // XR_UICASCADER_HPP