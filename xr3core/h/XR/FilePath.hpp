//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FILEPATH_HPP
#define XR_FILEPATH_HPP

#include "HardString.hpp"

#define XR_MAX_PATH 256

namespace XR
{

//==============================================================================
class FilePath: public HardString<XR_MAX_PATH>
{
public:
  // using
  using Base = HardString<XR_MAX_PATH>;

  // static
  static char const kDirSeparator = '/';
  static char const kExtSeparator = '.';

  // structors
  FilePath(char const* path = "")
  : Base(path)
  {
    NormalizeDirSeparators();
  }

  // general
  void NormalizeDirSeparators();

  void AppendDirSeparator();

  char const* GetNameExt() const;
  char const* GetExt() const;

  bool StartsWith(char const* str, size_t strSize) const
  {
    return strncmp(m_arBuffer, str, strSize) == 0;
  }

  bool StartsWith(char const* str) const
  {
    return StartsWith(str, strlen(str));
  }

  bool StartsWith(FilePath const& other) const
  {
    return StartsWith(other.m_arBuffer);
  }

  void Up();

  // operators
  FilePath& operator=(char const* path)
  {
    Base::operator=(path);
    NormalizeDirSeparators();
    return *this;
  }

  FilePath& operator/=(char const* path)
  {
    AppendDirSeparator();
    if (*path == kDirSeparator)
    {
      ++path;
    }
    *this += FilePath(path);
    return *this;
  }

  FilePath operator/(char const* path) const
  {
    FilePath newPath(*this);
    return newPath /= path;
  }
};

} // XR

#endif // XR_FILEPATH
