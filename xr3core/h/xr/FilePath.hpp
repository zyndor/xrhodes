#ifndef XR_FILEPATH_HPP
#define XR_FILEPATH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "HardString.hpp"

#define XR_MAX_PATH 256

namespace xr
{

//==============================================================================
///@brief File path abstraction with helpers for frequently used operations and
/// max supported size built in.
///@note Converts backslashes to kDirSeparator upon assignment and construction.
class FilePath: public HardString<XR_MAX_PATH>
{
public:
  // using
  using Base = HardString<XR_MAX_PATH>;

  // static
  static char const kDirSeparator;
  static char const kExtSeparator;

  // structors
  FilePath(char const* path = "")
  : Base(path)
  {
    NormalizeDirSeparators();
  }

  FilePath(Base const& rhs)
  : Base(rhs)
  {}

  // general
  ///@brief Replaces all backslashes with kDirSeparator.
  void NormalizeDirSeparators();

  ///@brief Appends a kDirSeparator to this only if it doesn't already end with one.
  void AppendDirSeparator();

  ///@return The filename and extension part of the path, if any.
  char const* GetNameExt() const;

  ///@return The filename and extension part of the path, if any.
  char* GetNameExt();

  ///@return The extension part of the path, which follows the last kExtSeparator
  /// in the filename, if any.
  char const* GetExt() const;

  ///@return The extension part of the path, which follows the last kExtSeparator
  /// in the filename, if any.
  char* GetExt();

  ///@return Does this start with the first @a strSize bytes of @a str?
  bool StartsWith(char const* str, size_t strSize) const
  {
    return strncmp(m_buffer, str, strSize) == 0;
  }

  ///@return Does this start with the @a str?
  bool StartsWith(char const* str) const
  {
    return StartsWith(str, strlen(str));
  }

  ///@return Does this start with @a other?
  bool StartsWith(FilePath const& other) const
  {
    return StartsWith(other.m_buffer, other.m_size);
  }

  ///@brief Removes the last name from the path. If the path ends with a
  /// kDirSeparator, then it removes that first.
  void Up();

  // operators
  FilePath& operator=(char const* path)
  {
    Base::operator=(path);
    NormalizeDirSeparators();
    return *this;
  }

  ///@brief Appends a kDirSeparator and the given path (with directory separators
  /// unified). Prevents duplicate dir separators at the boundary of this and
  /// the other path only.
  FilePath& operator/=(FilePath const& path)
  {
    AppendDirSeparator();
    char const* cpath = path.c_str();
    size_t size = path.size();
    while (*cpath == kDirSeparator)
    {
      ++cpath;
      --size;
    }
    return static_cast<FilePath&>(append(cpath, size));
  }

  FilePath& operator/=(char const* path)
  {
    AppendDirSeparator();
    size_t size = strlen(path);
    while (*path == kDirSeparator)
    {
      ++path;
      --size;
    }
    return static_cast<FilePath&>(append(path, size));
  }

  FilePath operator/(FilePath const& path) const
  {
    FilePath newPath(*this);
    return newPath /= path;
  }

  FilePath operator/(char const* path) const
  {
    FilePath newPath(*this);
    return newPath /= path;
  }
};

} // xr

#endif // XR_FILEPATH
