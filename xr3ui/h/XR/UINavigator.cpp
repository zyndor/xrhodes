#include  "UINavigator.hpp"

namespace XR
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
UINavigator::~UINavigator()
{}

//==============================================================================
bool  UINavigator::Add(UIElement* pElem, UIElement* pRelative,
        Direction dir)
{
  bool  success(true);
  if(pRelative == 0)
  {
    Entry e;
    memset(&e, 0x00, sizeof(Entry));

    m_entries[pElem] = e;
  }
  else
  {
    XR_ASSERT(UINavigator, dir < kNumDirections);
    EntryMap::iterator  iFind(m_entries.find(pRelative));
    success = iFind != m_entries.end();
    if(success)
    {
      Entry&  e(m_entries[pElem]);

      Direction opp(GetOpposite(dir));
      if(iFind->second.arNeighbors[dir] != 0)
      {
        // insert pElem between pRelative and its neighbor in the direction of insertion
        e.arNeighbors[dir] = iFind->second.arNeighbors[dir];

        EntryMap::iterator  iFind2(m_entries.find(iFind->second.arNeighbors[dir]));
        iFind2->second.arNeighbors[opp] = pElem;
      }

      e.arNeighbors[opp] = iFind->first;
      iFind->second.arNeighbors[dir] = pElem;
    }
  }
  return success;
}

//==============================================================================
bool  UINavigator::Remove(UIElement* pElem)
{
  EntryMap::iterator  iFind(m_entries.find(pElem));
  bool  success(iFind == m_entries.end());
  if(success)
  {
    EntryMap::iterator  iFindRelative[kNumDirections] =
    {
      m_entries.find(iFind->second.arNeighbors[LEFT]),
      m_entries.find(iFind->second.arNeighbors[UP]),
      m_entries.find(iFind->second.arNeighbors[RIGHT]),
      m_entries.find(iFind->second.arNeighbors[DOWN]),
    };

    EntryMap::iterator  iEnd(m_entries.end());

    if(iFindRelative[LEFT] != iEnd)
    {
      if(iFindRelative[RIGHT] != iEnd)
      {
        iFindRelative[LEFT]->second.arNeighbors[RIGHT] = iFindRelative[RIGHT]->first;
        iFindRelative[RIGHT]->second.arNeighbors[LEFT] = iFindRelative[LEFT]->first;
      }
      else
      {
        iFindRelative[LEFT]->second.arNeighbors[RIGHT] = 0;
      }
    }
    else if(iFindRelative[RIGHT] != iEnd)
    {
      iFindRelative[RIGHT]->second.arNeighbors[LEFT] = 0;
    }

    if(iFindRelative[UP] != iEnd)
    {
      if(iFindRelative[DOWN] != iEnd)
      {
        iFindRelative[UP]->second.arNeighbors[DOWN] = iFindRelative[DOWN]->first;
        iFindRelative[DOWN]->second.arNeighbors[UP] = iFindRelative[UP]->first;
      }
      else
      {
        iFindRelative[UP]->second.arNeighbors[DOWN] = 0;
      }
    }
    else if(iFindRelative[DOWN] != iEnd)
    {
      iFindRelative[DOWN]->second.arNeighbors[UP] = 0;
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
UIElement*  UINavigator::Get(UIElement* pRelative, Direction dir)
{
  XR_ASSERT(UINavigator, dir < kNumDirections);
  UIElement*  p(0);
  EntryMap::iterator  iFind(m_entries.find(pRelative));
  if(iFind != m_entries.end())
  {
    p = iFind->second.arNeighbors[dir];
  }
  return p;
}

} // XR
