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

  bool StartsWith(char const* str, size_t strSize) const
  {
    return strncmp(m_arBuffer, str, strSize) == 0;
  }

  bool StartsWith(char const* str) const
  {
    return StartsWith(str, strlen(str));
  }

  // operators
  FilePath& operator=(char const* path)
  {
    Base::operator=(path);
    NormalizeDirSeparators();
    return *this;
  }

  FilePath& operator/(char const* path)
  {
    AppendDirSeparator();
    *this += path;
    return *this;
  }
};

} // XR

#endif // XR_FILEPATH
