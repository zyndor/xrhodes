//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
Indexer2d::Indexer2d(int w, int h)
: m_width(w),
  m_height(h),
  m_size(_CalculateSize())
{}

//==============================================================================
Indexer2d::~Indexer2d()
{}

//==============================================================================
int  Indexer2d::_CalculateSize() const
{
  return m_width * m_height;
}

//==============================================================================
void  Indexer2d::SetWidth(int w)
{
  XR_ASSERT(Indexer2d, w >= 0);
  m_width = w;
  m_size = _CalculateSize();
}

//==============================================================================
void  Indexer2d::SetHeight(int h)
{
  XR_ASSERT(Indexer2d, h >= 0);
  m_height = h;
  m_size = _CalculateSize();
}

//==============================================================================
void  Indexer2d::SetSize(int w, int h)
{
  XR_ASSERT(Indexer2d, w >= 0);
  XR_ASSERT(Indexer2d, h >= 0);
  m_width = w;
  m_height = h;
  m_size = _CalculateSize();
}

//==============================================================================
int  Indexer2d::ToIndex(int x, int y) const
{
  return x + y * m_width;
}

//==============================================================================
void  Indexer2d::ToCoordinates(int i, int& x, int& y) const
{
  XR_ASSERT(Indexer2d, m_width > 0);
  y = i / m_width;
  x = i - (y * m_width);
}

}  // end namespace XR
