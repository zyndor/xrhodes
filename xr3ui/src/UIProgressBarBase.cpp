//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIProgressBarBase.hpp"

namespace xr
{

//==============================================================================
UIProgressBarBase::UIProgressBarBase( FillDir fd )
: UIImage(),
  m_fillDir(fd),
  m_percentage(.0f)
{}

//==============================================================================
UIProgressBarBase::~UIProgressBarBase()
{}

//==============================================================================
float UIProgressBarBase::GetPercentage() const
{
  return m_percentage;
}

//==============================================================================
UIProgressBarBase::FillDir UIProgressBarBase::GetFillDirection() const
{
  return m_fillDir;
}

//==============================================================================
void UIProgressBarBase::SetFillDirection( FillDir fd )
{
  if (m_fillDir != fd)
  {
    m_fillDir = fd;
  }
}

//==============================================================================
void UIProgressBarBase::SetPercentage( float perc )
{
  if (perc < .0f)
  {
    perc = .0f;
  }
  else if (perc > 1.0f)
  {
    perc = 1.0f;
  }

  m_percentage = perc;
}

} // xr
