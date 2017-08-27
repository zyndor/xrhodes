//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/FilePath.hpp>

namespace XR
{

//==============================================================================
void FilePath::NormalizeDirSeparators()
{
  char* p = m_arBuffer;
  while (p = strchr(p, '\\'))
  {
    *p = kDirSeparator;
    ++p;
  }
}

//==============================================================================
void FilePath::AppendDirSeparator()
{
  if (m_arBuffer[size() - 1] != kDirSeparator)
  {
    *this += kDirSeparator;
  }
}

//==============================================================================
char const* FilePath::GetNameExt() const
{
  char const* p = rfind(kDirSeparator);
  if (p)
  {
    ++p;
  }
  return p;
}

//==============================================================================
void FilePath::Up()
{
  if (char* p = rfind(kDirSeparator))
  {
    *p = '\0';
    if (p - m_arBuffer < kCapacity && p[1] == '\0')
    {
      Up();
    }
  }
}

} // XR
