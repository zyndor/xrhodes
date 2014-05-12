#include "UIVerticalLayout.hpp"

namespace XR
{

UIVerticalLayout::UIVerticalLayout()
: UIGrowingLayoutBase()
{}

UIVerticalLayout::~UIVerticalLayout()
{}


void UIVerticalLayout::_AlignElement( UIElement* pElem )
{
  if (m_align != AL_NOALIGN)
  {
    int16 xElem(x);
    switch (m_align)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      xElem += (w - pElem->w) / 2;
      break;

    case  AL_HIGH:
      xElem += w - pElem->w;
      break;
    }
    pElem->x = xElem;
  }

  int16 yElem(0);
  if (m_lElements.size() > 0)
  {
    UIElement*  pLast(m_lElements.back());
    yElem = pLast->y;
    if (m_growDir == GD_POSITIVE)
    {
      yElem += pLast->h + m_spacing;
    }
    else
    {
      yElem -= (pElem->h + m_spacing);
    }
  }
  else
  {
    yElem = y;
    if (m_growDir != GD_POSITIVE)
    {
      yElem += h - pElem->h;
    }
  }
  pElem->y = yElem;

  pElem->OnChange();
}

void UIVerticalLayout::_SetHeightToContent()
{
  int hNew = 0;
  for (ElementList::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    hNew += (*i0)->h;
  }

  if (m_lElements.size() > 0)
  {
    hNew += (m_lElements.size() - 1) * m_spacing;
  }

  h = hNew;
}

} // XR
