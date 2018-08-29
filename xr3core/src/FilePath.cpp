//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/FilePath.hpp"

namespace xr
{

//==============================================================================
const char FilePath::kDirSeparator = '/';
const char FilePath::kExtSeparator = '.';

//==============================================================================
void FilePath::NormalizeDirSeparators()
{
  char* p = m_buffer;
  while ((p = strchr(p, '\\')) != nullptr)
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

//==============================================================================
FilePath FilePath::Canonical() const
{
  FilePath canonical;
  char const* token = m_buffer;
  char const* end = strchr(token, kDirSeparator);
  while (end)
  {
    if (end && end - token > 1) // ignore repeated slashes
    {
      if (strncmp("..", token, 2) != 0)
      {
        canonical.append(token, end - token);
      }
      else
      {
        canonical.Up();
      }
      canonical.AppendDirSeparator();
    }
    token = end + 1;
    end = strchr(token, kDirSeparator);
  }

  end = m_buffer + m_size;
  if (end - token > 1)
  {
    if (strncmp("..", token, 2) != 0)
    {
      canonical.append(token, end - token);
    }
    else
    {
      canonical.Up();
    }
  }

  return canonical;
}

} // xr
