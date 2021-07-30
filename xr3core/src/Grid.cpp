//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Grid.hpp"
#include "xr/debug.hpp"
#include <cmath>

namespace xr
{

//==============================================================================
///@brief The minimum size allowed for a grid.
const float  Grid::kMinSize(.1f);

//==============================================================================
Grid::Grid()
: m_size(.0f),
  m_halfSize(.0f),
  m_sizeRec(.0f)
{
  SetSize(1.0f);
}

//==============================================================================
Grid::Grid(float size)
: m_size(.0f),
  m_halfSize(.0f),
  m_sizeRec(.0f)
{
  SetSize(size);
}

//==============================================================================
Grid::~Grid()
{}

//==============================================================================
void  Grid::SetSize(float size)
{
  XR_ASSERT(Grid, size > kMinSize);

  m_size = size;
  m_halfSize = size * .5f;
  m_sizeRec = 1.0f / size;
}

//==============================================================================
int  Grid::ToGrid(float world) const
{
  return static_cast<int>(floorf(world * m_sizeRec));
}

}  // xr
