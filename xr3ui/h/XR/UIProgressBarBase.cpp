//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIProgressBarBase.hpp"

namespace XR
{

//==============================================================================
UIProgressBarBase::UIProgressBarBase( FillDir fd )
: UIImage(),
  m_percentage(.0f),
  m_fillDir(fd)
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

} // XR
