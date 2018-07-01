//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UICascader.hpp"
#include <limits>

namespace xr
{

//==============================================================================
UICascader::UICascader()
: UIAligner(),
  m_horizontalOffset(0),
  m_verticalOffset(0)
{}

//==============================================================================
void UICascader::AlignChildImpl( UIElement* elem )
{
  if (m_hAlign != AL_NOALIGN)
  {
    int x1 = static_cast<int32_t>(x + m_horizontalOffset * m_children.size());

    switch (m_hAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      x1 += (w - elem->w) / 2;
      break;

    case  AL_HIGH:
      x1 += w - elem->w;
      break;

    case  AL_NOALIGN:
      break;
    }

    elem->x = x1;
  }

  if (m_vAlign)
  {
    int y1 = static_cast<int32_t>(y + m_verticalOffset * m_children.size());

    switch (m_vAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      y1 += (h - elem->h) / 2;
      break;

    case  AL_HIGH:
      y1 += h - elem->h;
      break;

    case  AL_NOALIGN:
      break;
    }

    elem->y = y1;
  }

  elem->OnChange();
}

//==============================================================================
void UICascader::SetHorizontalOffset( int val )
{
  if (val != m_horizontalOffset)
  {
    m_horizontalOffset = val;
    OnChange();
  }
}

//==============================================================================
void UICascader::SetVerticalOffset( int val )
{
  if (val != m_verticalOffset)
  {
    m_verticalOffset = val;
    OnChange();
  }
}

//==============================================================================
void UICascader::SetWidthToContentImpl()
{
  int xMin = std::numeric_limits<int>::max();
  int xMax = std::numeric_limits<int>::min();
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    const UIElement*  elem(*i0);
    if (elem->x < xMin)
    {
      xMin = elem->x;
    }

    int right(elem->CalculateRight());
    if (right > xMax)
    {
      xMax = right;
    }
  }

  w = xMax - xMin;
}

//==============================================================================
void UICascader::SetHeightToContentImpl()
{
  int yMin(std::numeric_limits<int>::max());
  int yMax(std::numeric_limits<int>::min());
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    const UIElement*  elem(*i0);
    if (elem->y < yMin)
    {
      yMin = elem->y;
    }

    int bottom(elem->CalculateBottom());
    if (bottom > yMax)
    {
      yMax = bottom;
    }
  }

  h = yMax - yMin;
}

} // xr
