#ifndef XR_UINAVIGATOR_HPP
#define XR_UINAVIGATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <map>
#include "UIElement.hpp"

namespace xr
{

//==============================================================================
///@brief Provides a facility to describe UIElements' relative cardinal directions,
/// allowing the creation of a walkable graph of focus.
class UINavigator
{
public:
  // types
  enum  Direction
  {
    LEFT,
    UP,
    RIGHT,
    DOWN,
    kNumDirections
  };

  // static
  static Direction  GetOpposite(Direction dir);

  // structors
  UINavigator();

  // general
  bool  Add(UIElement* elem, UIElement* relativeTo, Direction dir);
  bool  Remove(UIElement* elem);
  void  Clear();

  ///@return The element that's in @a dir direction from the UIElement
  /// @a relativeTo. nullptr if there isn't such an element.
  UIElement*  Get(UIElement* relativeTo, Direction dir);

protected:
  // types
  struct  Entry
  {
    using Map = std::map<UIElement*, Entry>;

    UIElement*  neighbors[kNumDirections];
  };

  // data
  Entry::Map  m_entries;
};

} // xr

#endif  //XR_UINAVIGATOR_HPP
