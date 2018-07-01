//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIVerticalLayout.hpp"

namespace xr
{

//==============================================================================
UIVerticalLayout::UIVerticalLayout()
: UIGrowingLayoutBase()
{}

//==============================================================================
void UIVerticalLayout::AlignChildImpl( UIElement* elem )
{
  if (m_align != AL_NOALIGN)
  {
    int xElem(x);
    switch (m_align)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      xElem += (w - elem->w) / 2;
      break;

    case  AL_HIGH:
      xElem += w - elem->w;
      break;

    case  AL_NOALIGN:
      break;
    }
    elem->x = xElem;
  }

  int yElem(0);
  if (m_children.size() > 0)
  {
    UIElement*  pLast(m_children.back());
    yElem = pLast->y;
    if (m_growDir == GD_POSITIVE)
    {
      yElem += pLast->h + m_spacing;
    }
    else
    {
      yElem -= (elem->h + m_spacing);
    }
  }
  else
  {
    yElem = y;
    if (m_growDir != GD_POSITIVE)
    {
      yElem += h - elem->h;
    }
  }
  elem->y = yElem;

  elem->OnChange();
}

//==============================================================================
void UIVerticalLayout::SetHeightToContentImpl()
{
  int32_t hNew = 0;
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    hNew += (*i0)->h;
  }

  if (m_children.size() > 0)
  {
    hNew += static_cast<int32_t>(m_children.size() - 1) * m_spacing;
  }

  h = hNew;
}

} // xr
