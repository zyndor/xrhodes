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
  char* p = m_buffer;
  while (p = strchr(p, '\\'))
  {
    *p = kDirSeparator;
    ++p;
  }
}

//==============================================================================
void FilePath::AppendDirSeparator()
{
  if (m_buffer[size() - 1] != kDirSeparator)
  {
    append(&kDirSeparator, 1);
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
  else
  {
    p = m_buffer;
  }
  return p;
}

//==============================================================================
char const* FilePath::GetExt() const
{
  auto ext = strrchr(GetNameExt(), kExtSeparator);
  if(ext)
  {
    ++ext;
  }
  return ext;
}

//==============================================================================
void FilePath::Up()
{
  if (char* p = rfind(kDirSeparator))
  {
    *p = '\0';
    if (p - m_buffer < kCapacity && p[1] == '\0')
    {
      Up();
    }
  }
}

} // XR
