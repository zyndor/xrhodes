#include "UIAligner.hpp"

namespace XR
{

//==============================================================================
UIAligner::UIAligner()
: UIContainer(),
  m_hAlign(AL_LOW),
  m_vAlign(AL_LOW)
{}

//==============================================================================
UIAligner::~UIAligner()
{}

//==============================================================================
void UIAligner::_AlignElement( UIElement* pElem )
{
  if(m_hAlign != AL_NOALIGN)
  {
    int x1(x);

    switch(m_hAlign)
    {
    case  AL_LOW:
      // we're good
      break;

    case  AL_CENTER:
      x1 += (w - pElem->w) / 2;
      break;

    case  AL_HIGH:
      x1 += w - pElem->w;
      break;
    }

    pElem->x = x1;
  }

  if(m_vAlign != AL_NOALIGN)
  {
    int y1(y);

    switch(m_vAlign)
    {
    case  AL_LOW:
      // we're good
      break;

    case  AL_CENTER:
      y1 += (h - pElem->h) / 2;
      break;

    case  AL_HIGH:
      y1 += h - pElem->h;
      break;
    }
    
    pElem->y = y1;
  }

  pElem->OnChange();
}

//==============================================================================
void UIAligner::SetHorizontalAlignment( Alignment hAlign )
{
  if(m_hAlign != hAlign)
  {
    m_hAlign = hAlign;
    OnChange();
  }
}

//==============================================================================
void UIAligner::SetVerticalAlignment( Alignment vAlign )
{
  if(m_vAlign != vAlign)
  {
    m_vAlign = vAlign;
    OnChange();
  }
}

} // XR