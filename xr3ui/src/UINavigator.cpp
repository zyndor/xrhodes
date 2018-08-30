//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UINavigator.hpp"

namespace xr
{

//==============================================================================
UINavigator::Direction  UINavigator::GetOpposite(Direction dir)
{
  XR_ASSERT(UINavigator, dir < kNumDirections);
  return static_cast<Direction>((dir + 2) % kNumDirections);
}

//==============================================================================
UINavigator::UINavigator()
: m_entries()
{}

//==============================================================================
bool  UINavigator::Add(UIElement* elem, UIElement* relativeTo,
        Direction dir)
{
  bool  success = true;
  if(!relativeTo)
  {
    Entry e;
    memset(&e, 0x00, sizeof(Entry));

    m_entries[elem] = e;
  }
  else
  {
    XR_ASSERT(UINavigator, dir < kNumDirections);
    Entry::Map::iterator  iFind = m_entries.find(relativeTo);
    success = iFind != m_entries.end();
    if(success)
    {
      Entry&  e = m_entries[elem];

      auto& sameDirNeighbor = iFind->second.neighbors[dir];
      const Direction opp = GetOpposite(dir);
      if(sameDirNeighbor)
      {
        // insert elem between relativeTo and its neighbor in the direction of insertion
        e.neighbors[dir] = sameDirNeighbor;

        Entry::Map::iterator  iFindSameDirNeighbor = m_entries.find(sameDirNeighbor);
        iFindSameDirNeighbor->second.neighbors[opp] = elem;
      }

      e.neighbors[opp] = iFind->first;
      sameDirNeighbor = elem;
    }
  }
  return success;
}

//==============================================================================
bool  UINavigator::Remove(UIElement* elem)
{
  Entry::Map::iterator  iFind = m_entries.find(elem);
  bool  success = iFind == m_entries.end();
  if(success)
  {
    Entry::Map::iterator  iFindRelative[kNumDirections] =
    {
      m_entries.find(iFind->second.neighbors[LEFT]),
      m_entries.find(iFind->second.neighbors[UP]),
      m_entries.find(iFind->second.neighbors[RIGHT]),
      m_entries.find(iFind->second.neighbors[DOWN]),
    };

    Entry::Map::iterator  iEnd = m_entries.end();
    if(iFindRelative[LEFT] != iEnd)
    {
      if(iFindRelative[RIGHT] != iEnd)
      {
        iFindRelative[LEFT]->second.neighbors[RIGHT] = iFindRelative[RIGHT]->first;
        iFindRelative[RIGHT]->second.neighbors[LEFT] = iFindRelative[LEFT]->first;
      }
      else
      {
        iFindRelative[LEFT]->second.neighbors[RIGHT] = nullptr;
      }
    }
    else if(iFindRelative[RIGHT] != iEnd)
    {
      iFindRelative[RIGHT]->second.neighbors[LEFT] = nullptr;
    }

    if(iFindRelative[UP] != iEnd)
    {
      if(iFindRelative[DOWN] != iEnd)
      {
        iFindRelative[UP]->second.neighbors[DOWN] = iFindRelative[DOWN]->first;
        iFindRelative[DOWN]->second.neighbors[UP] = iFindRelative[UP]->first;
      }
      else
      {
        iFindRelative[UP]->second.neighbors[DOWN] = nullptr;
      }
    }
    else if(iFindRelative[DOWN] != iEnd)
    {
      iFindRelative[DOWN]->second.neighbors[UP] = nullptr;
    }
  }
  return success;
}

//==============================================================================
void  UINavigator::Clear()
{
  m_entries.clear();
}

//==============================================================================
UIElement*  UINavigator::Get(UIElement* relativeTo, Direction dir)
{
  XR_ASSERT(UINavigator, dir < kNumDirections);
  UIElement*  elem = nullptr;
  Entry::Map::iterator  iFind = m_entries.find(relativeTo);
  if(iFind != m_entries.end())
  {
    elem = iFind->second.neighbors[dir];
  }
  return elem;
}

} // xr
