//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Indexer2d.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
Indexer2d::Indexer2d()
: m_width(0),
  m_height(0),
  m_size(0)
{}

//==============================================================================
Indexer2d::Indexer2d(int32_t w, int32_t h)
: m_width(w),
  m_height(h),
  m_size(CalculateSize())
{}

//==============================================================================
Indexer2d::~Indexer2d()
{}

//==============================================================================
int32_t  Indexer2d::CalculateSize() const
{
  return m_width * m_height;
}

//==============================================================================
void  Indexer2d::SetWidth(int32_t w)
{
  XR_ASSERT(Indexer2d, w >= 0);
  m_width = w;
  m_size = CalculateSize();
}

//==============================================================================
void  Indexer2d::SetHeight(int32_t h)
{
  XR_ASSERT(Indexer2d, h >= 0);
  m_height = h;
  m_size = CalculateSize();
}

//==============================================================================
void  Indexer2d::SetSize(int32_t w, int32_t h)
{
  XR_ASSERT(Indexer2d, w >= 0);
  XR_ASSERT(Indexer2d, h >= 0);
  m_width = w;
  m_height = h;
  m_size = CalculateSize();
}

//==============================================================================
int32_t  Indexer2d::ToIndex(int32_t x, int32_t y) const
{
  return x + y * m_width;
}

//==============================================================================
void  Indexer2d::ToCoordinates(int32_t i, int32_t& x, int32_t& y) const
{
  XR_ASSERT(Indexer2d, m_width > 0);
  y = i / m_width;
  x = i - (y * m_width);
}

}  // end namespace XR
