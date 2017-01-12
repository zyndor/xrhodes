//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIGrowingLayoutBase.hpp"

namespace XR
{

//==============================================================================
int UIGrowingLayoutBase::s_defaultSpacing(0);

//==============================================================================
int UIGrowingLayoutBase::GetDefaultSpacing()
{
  return s_defaultSpacing;
}

//==============================================================================
void UIGrowingLayoutBase::SetDefaultSpacing( int spacing )
{
  XR_ASSERT(UIGrowingLayoutBase, spacing >= 0);
  s_defaultSpacing = spacing;
}

//==============================================================================
UIGrowingLayoutBase::UIGrowingLayoutBase()
: UIContainer(),
  m_growDir(GD_POSITIVE),
  m_align(AL_LOW),
  m_spacing(s_defaultSpacing)
{}

//==============================================================================
UIGrowingLayoutBase::~UIGrowingLayoutBase()
{}

//==============================================================================
void UIGrowingLayoutBase::SetGrowDir( GrowDir growDir )
{
  m_growDir = growDir;
  OnChange();
}

//==============================================================================
void UIGrowingLayoutBase::SetSpacing( int spacing )
{
  XR_ASSERTMSG(IW_ASSERTION_CHANNEL_DEFAULT, spacing >= 0, ("Invalid negative spacing value."));

  m_spacing = spacing;
  OnChange();
}

//==============================================================================
void UIGrowingLayoutBase::SetAlignment( Alignment align )
{
  m_align = align;
}

} // XR
