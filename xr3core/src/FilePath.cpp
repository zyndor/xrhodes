//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/FilePath.hpp"

namespace XR
{

//==============================================================================
const char FilePath::kDirSeparator = '/';
const char FilePath::kExtSeparator = '.';

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
  auto p = rfind(kDirSeparator);
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
char* FilePath::GetNameExt()
{
  auto p = rfind(kDirSeparator);
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
  if (ext)
  {
    ++ext;
  }
  return ext;
}

//==============================================================================
char* FilePath::GetExt()
{
  auto ext = strrchr(GetNameExt(), kExtSeparator);
  if (ext)
  {
    ++ext;
  }
  return ext;
}

//==============================================================================
void FilePath::Up()
{
  char* p = m_buffer + (m_size - 1);
  bool sizeDirty = false;
  if (*p == kDirSeparator)
  {
    *p = '\0';
    sizeDirty = true;
  }

  if (char* p = rfind(kDirSeparator))
  {
    *p = '\0';
    sizeDirty = true;
  }

  if (sizeDirty)
  {
    UpdateSize();
  }
}

} // XR
