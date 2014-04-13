#include "UIGrowingLayoutBase.hpp"

namespace XR
{


int16 UIGrowingLayoutBase::s_defaultSpacing(0);

int16 UIGrowingLayoutBase::GetDefaultSpacing()
{
  return s_defaultSpacing;
}

void UIGrowingLayoutBase::SetDefaultSpacing( int16 spacing )
{
  XR_ASSERT(UIGrowingLayoutBase, spacing >= 0);
  s_defaultSpacing = spacing;
}


UIGrowingLayoutBase::UIGrowingLayoutBase()
: UIContainer(),
  m_growDir(GD_POSITIVE),
  m_align(AL_LOW),
  m_spacing(s_defaultSpacing)
{}

UIGrowingLayoutBase::~UIGrowingLayoutBase()
{}

void UIGrowingLayoutBase::SetGrowDir( GrowDir growDir )
{
  m_growDir = growDir;
  OnChange();
}

void UIGrowingLayoutBase::SetSpacing( int16 spacing )
{
  XR_ASSERTMSG(IW_ASSERTION_CHANNEL_DEFAULT, spacing >= 0, ("Invalid negative spacing value."));

  m_spacing = spacing;
  OnChange();
}

void UIGrowingLayoutBase::SetAlignment( Alignment align )
{
  m_align = align;
}

} // XR
