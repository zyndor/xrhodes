//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Indexer2d.hpp>
#include <XR/debug.hpp>

namespace XR
{

//==============================================================================
Indexer2D::Indexer2D()
: m_width(0),
  m_height(0),
  m_size(0)
{}

//==============================================================================
Indexer2D::Indexer2D(int w, int h)
: m_width(w),
  m_height(h),
  m_size(_CalculateSize())
{}

//==============================================================================
Indexer2D::~Indexer2D()
{}

//==============================================================================
int  Indexer2D::_CalculateSize() const
{
  return m_width * m_height;
}

//==============================================================================
void  Indexer2D::SetWidth(int w)
{
  XR_ASSERT(Indexer2D, w >= 0);
  m_width = w;
  m_size = _CalculateSize();
}

//==============================================================================
void  Indexer2D::SetHeight(int h)
{
  XR_ASSERT(Indexer2D, h >= 0);
  m_height = h;
  m_size = _CalculateSize();
}

//==============================================================================
void  Indexer2D::SetSize(int w, int h)
{
  XR_ASSERT(Indexer2D, w >= 0);
  XR_ASSERT(Indexer2D, h >= 0);
  m_width = w;
  m_height = h;
  m_size = _CalculateSize();
}

//==============================================================================
int  Indexer2D::ToIndex(int x, int y) const
{
  return x + y * m_width;
}

//==============================================================================
void  Indexer2D::ToCoordinates(int i, int& x, int& y) const
{
  XR_ASSERT(Indexer2D, m_width > 0);
  x = i % m_width;
  y = i / m_width;
}

}  // end namespace XR
