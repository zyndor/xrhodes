#ifndef XR_RANGETREE_HPP
#define XR_RANGETREE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
//==============================================================================
#include <cstdint>

namespace xr
{

//==============================================================================
// A graph of ranges. Adding a new range overlapping with pre-existing ranges
// will result in the extending (and if multiple pre-existing ranges are
// overlapped by a new one, merging) of pre-existing ranges, meaning that
// despite its binary tree like appearance, the graph never has inner leaves.
struct RangeTree
{
  uint32_t start; // inclusive
  uint32_t end; // exclusive
  RangeTree* low = nullptr;
  RangeTree* high = nullptr;
  RangeTree* parent = nullptr; // no ownership

  RangeTree()
  {}

  RangeTree(uint32_t r0, uint32_t r1)
  : start(r0),
    end(r1)
  {}

  RangeTree(RangeTree const& range, RangeTree* parent = nullptr)
  : start(range.start),
    end(range.end),
    parent(parent)
  {}

  ~RangeTree()
  {
    delete low;
    delete high;
  }

  void Add(RangeTree const& rt)
  {
    if (rt.end >= start && rt.start <= end) // handle overlap
    {
      if (rt.start < start) // extend below
      {
        start = rt.start;
        MergeLow();
      }

      if (rt.end > end) // extend above
      {
        end = rt.end;
        MergeHigh();
      }
    }
    else if (rt.end < start) // clean lower
    {
      if (low && low->end >= rt.start) // lower than low
      {
        low->Add(rt);
      }
      else // higher than low (or no low) - insert above low.
      {
        auto tmp = low;
        low = new RangeTree(rt, this);
        if (tmp)
        {
          tmp->parent = low;
          low->low = tmp;
        }
      }
    }
    else if (rt.start > end)  // clean higher
    {
      if (high && high->start <= rt.end) // higher than high
      {
        high->Add(rt);
      }
      else // lower than high (or no high) - insert below high.
      {
        auto tmp = high;
        high = new RangeTree(rt, this);
        if (tmp)
        {
          tmp->parent = high;
          high->high = tmp;
        }
      }
    }
  }

  void MergeLow()
  {
    // Find new start from lower nodes colliding with our range, and remove
    // these colliding nodes.
    while (low && low->end >= start)
    {
      if (low->start < start)
      {
        start = low->start;
      }

      auto lower = low->low;
      if (lower)
      {
        low->low = nullptr;
        lower->parent = this;
      }
      delete low;
      low = lower;
    }
  }

  void MergeHigh()
  {
    // Find new start from lower nodes colliding with our range, and remove
    // these colliding nodes.
    while (high && high->start <= end)
    {
      if (high->end > end)
      {
        end = high->end;
      }

      auto higher = high->high;
      if (higher)
      {
        high->high = nullptr;
        higher->parent = this;
      }
      delete high;
      high = higher;
    }
  }

  RangeTree const* Lowest() const
  {
    auto t = this;
    while (t->low)
    {
      t = t->low;
    }
    return t;
  }

  RangeTree const* Highest() const
  {
    auto t = this;
    while (t->high)
    {
      t = t->high;
    }
    return t;
  }

  RangeTree const* Lower() const
  {
    RangeTree const* t = low;
    if (t)  // get the highest of the low node, if any.
    {
      t = t->Highest();
    }
    else
    {
      // if not, traverse the list of parents until we get to one whose low node
      // isn't the one most recently processed.
      auto p = parent;
      while (p && p->low == t)
      {
        t = p;
        p = p->parent;
      }
    }
    return t;
  }

  RangeTree const* Higher() const
  {
    RangeTree const* t = high;
    if (t)  // get the lowest of the high node, if any.
    {
      t = t->Lowest();
    }
    else
    {
      // if not, traverse the list of parents until we get to one whose high node
      // isn't the one most recently processed.
      auto p = parent;
      while (p && p->high == t)
      {
        t = p;
        p = p->parent;
      }
    }
    return t;
  }
};

}

#endif
