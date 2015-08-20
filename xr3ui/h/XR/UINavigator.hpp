//-
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UINAVIGATOR_HPP
#define XR_UINAVIGATOR_HPP

#include  <map>
#include  <XR/UIElement.hpp>

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
  bool  Add(XR::UIElement* pElem, XR::UIElement* pRelative, Direction dir);
  bool  Remove(XR::UIElement* pElem);
  void  Clear();

  XR::UIElement*  Get(XR::UIElement* pRelative, Direction dir);

protected:
  // types
  struct  Entry
  {
    XR::UIElement*  arNeighbors[kNumDirections];
  };

  typedef std::map<XR::UIElement*, Entry> EntryMap;

  // data
  EntryMap  m_entries;
};

} // XR

#endif  //XR_UINAVIGATOR_HPP
