//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UICascader.hpp"
#include <limits>

namespace XR
{

//==============================================================================
UICascader::UICascader()
: UIAligner(),
  m_horizontalOffset(0),
  m_verticalOffset(0)
{}

//==============================================================================
UICascader::~UICascader()
{}

//==============================================================================
void UICascader::_AlignElement( UIElement* pElem )
{
  if (m_hAlign != AL_NOALIGN)
  {
    int x1 = static_cast<int32_t>(x + m_horizontalOffset * m_lElements.size());

    switch (m_hAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      x1 += (w - pElem->w) / 2;
      break;

    case  AL_HIGH:
      x1 += w - pElem->w;
      break;

    case  AL_NOALIGN:
      break;
    }

    pElem->x = x1;
  }

  if (m_vAlign)
  {
    int y1 = static_cast<int32_t>(y + m_verticalOffset * m_lElements.size());

    switch (m_vAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      y1 += (h - pElem->h) / 2;
      break;

    case  AL_HIGH:
      y1 += h - pElem->h;
      break;

    case  AL_NOALIGN:
      break;
    }

    pElem->y = y1;
  }

  pElem->OnChange();
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
void UICascader::_SetWidthToContent()
{
  int xMin = std::numeric_limits<int>::max();
  int xMax = std::numeric_limits<int>::min();
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    const UIElement*  pElem(*i0);
    if (pElem->x < xMin)
    {
      xMin = pElem->x;
    }

    int right(pElem->CalculateRight());
    if (right > xMax)
    {
      xMax = right;
    }
  }

  w = xMax - xMin;
}

//==============================================================================
void UICascader::_SetHeightToContent()
{
  int yMin(std::numeric_limits<int>::max());
  int yMax(std::numeric_limits<int>::min());
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    const UIElement*  pElem(*i0);
    if (pElem->y < yMin)
    {
      yMin = pElem->y;
    }

    int bottom(pElem->CalculateBottom());
    if (bottom > yMax)
    {
      yMax = bottom;
    }
  }

  h = yMax - yMin;
}

} // XR
