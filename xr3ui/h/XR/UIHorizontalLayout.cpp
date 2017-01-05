#include "UIHorizontalLayout.hpp"

namespace XR
{

//==============================================================================
UIHorizontalLayout::UIHorizontalLayout()
: UIGrowingLayoutBase()
{}

//==============================================================================
UIHorizontalLayout::~UIHorizontalLayout()
{}

//==============================================================================
void UIHorizontalLayout::_AlignElement( UIElement* pElem )
{
  if (m_align != AL_NOALIGN)
  {
    int16_t yElem(y);
    switch (m_align)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      yElem += (h - pElem->h) / 2;
      break;

    case  AL_HIGH:
      yElem += h - pElem->h;
      break;
    }

    pElem->y = yElem;
  }

  int16_t xElem(0);
  if (m_lElements.size() > 0)
  {
    UIElement*  pLast(m_lElements.back());
    xElem = pLast->x;
    if (m_growDir == GD_POSITIVE)
    {
      xElem += pLast->w + m_spacing;
    }
    else
    {
      xElem -= (pElem->w + m_spacing);
    }
  }
  else
  {
    xElem = x;
    if (m_growDir != GD_POSITIVE)
    {
      xElem += w - pElem->w;
    }
  }
  pElem->x = xElem;

  pElem->OnChange();
}

//==============================================================================
void UIHorizontalLayout::_SetWidthToContent()
{
  int wNew = 0;
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    wNew += (*i0)->w;
  }

  if (m_lElements.size() > 0)
  {
    wNew += (m_lElements.size() - 1) * m_spacing;
  }

  w = wNew;
}

} // XR
