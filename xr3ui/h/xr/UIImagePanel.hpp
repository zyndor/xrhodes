#ifndef XR_UIIMAGEPANEL_HPP
#define XR_UIIMAGEPANEL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIImage.hpp"

namespace xr
{

//==============================================================================
///@brief An image which gets rendered as a 3x3 tiles, with corners and middle
/// bits, the latter of which being stretched to fit the size of the element.
/// The size / proportion of the tiles is controlled by the vSplit and hSplit
/// data members, which are percent values in the [.0, 1.0] range.
class UIImagePanel:  public UIImage
{
public:
  // data
  float hSplit;
  float vSplit;

  // structors
  UIImagePanel();

  // general use
  void Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIIMAGEPANEL_HPP
