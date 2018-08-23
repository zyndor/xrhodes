//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIHorizontalLayout.hpp"

namespace xr
{

//==============================================================================
UIHorizontalLayout::UIHorizontalLayout()
: UIGrowingLayoutBase()
{}

//==============================================================================
void UIHorizontalLayout::AlignChildImpl( UIElement* elem )
{
  if (m_align != AL_NOALIGN)
  {
    int16_t yElem = y;
    switch (m_align)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      yElem += (h - elem->h) / 2;
      break;

    case  AL_HIGH:
      yElem += h - elem->h;
      break;

    case  AL_NOALIGN:
      break;
    }

    elem->y = yElem;
  }

  int16_t xElem = 0;
  if (m_children.size() > 0)
  {
    UIElement* pLast = m_children.back();
    xElem = pLast->x;
    if (m_growDir == GD_POSITIVE)
    {
      xElem += pLast->w + m_spacing;
    }
    else
    {
      xElem -= (elem->w + m_spacing);
    }
  }
  else
  {
    xElem = x;
    if (m_growDir != GD_POSITIVE)
    {
      xElem += w - elem->w;
    }
  }
  elem->x = xElem;

  elem->OnChange();
}

//==============================================================================
void UIHorizontalLayout::SetWidthToContentImpl()
{
  int32_t wNew = 0;
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    wNew += (*i0)->w;
  }

  if (m_children.size() > 0)
  {
    wNew += static_cast<int32_t>(m_children.size() - 1) * m_spacing;
  }

  w = wNew;
}

} // xr
