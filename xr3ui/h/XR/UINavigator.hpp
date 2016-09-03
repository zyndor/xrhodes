//-
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#ifndef XR_UINAVIGATOR_HPP
#define XR_UINAVIGATOR_HPP

#include  <map>
#include  "UIElement.hpp"

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
  bool  Add(UIElement* pElem, UIElement* pRelative, Direction dir);
  bool  Remove(UIElement* pElem);
  void  Clear();

  UIElement*  Get(UIElement* pRelative, Direction dir);

protected:
  // types
  struct  Entry
  {
    UIElement*  arNeighbors[kNumDirections];
  };

  typedef std::map<UIElement*, Entry> EntryMap;

  // data
  EntryMap  m_entries;
};

} // XR

#endif  //XR_UINAVIGATOR_HPP
