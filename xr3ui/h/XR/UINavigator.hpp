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

namespace XR
{

//==============================================================================
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
  ~UINavigator();

  // general
  bool  Add(UIElement* elem, UIElement* relativeTo, Direction dir);
  bool  Remove(UIElement* elem);
  void  Clear();

  UIElement*  Get(UIElement* relativeTo, Direction dir);

protected:
  // types
  struct  Entry
  {
    UIElement*  neighbors[kNumDirections];
  };

  typedef std::map<UIElement*, Entry> EntryMap;

  // data
  EntryMap  m_entries;
};

} // XR

#endif  //XR_UINAVIGATOR_HPP
