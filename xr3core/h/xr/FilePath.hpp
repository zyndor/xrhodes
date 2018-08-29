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
///@note Does not actually use the file system for any of its operations.
class FilePath: public HardString<XR_MAX_PATH>
{
public:
  // using
  using Base = HardString<XR_MAX_PATH>;

  // static
  static char const kDirSeparator;
  static char const kExtSeparator;

  // structors
  FilePath(char const* path = "");
  FilePath(char const* path, size_t len);
  FilePath(Base const& rhs);

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
  bool StartsWith(char const* str, size_t strSize) const;

  ///@return Does this start with the @a str?
  bool StartsWith(char const* str) const;

  ///@return Does this start with @a other?
  bool StartsWith(FilePath const& other) const;

  ///@brief Removes the last name from the path. If the path ends with a
  /// kDirSeparator, then it removes that first.
  void Up();

  ///@brief Supposed to convert an absolute path to canonical. In effect,
  /// it will return a version of the path that has all backtracking (..)
  /// resolved.
  FilePath Canonical() const;

  // operators
  FilePath& operator=(char const* path);

  ///@brief Appends a kDirSeparator and the given path (with directory separators
  /// unified). Prevents duplicate dir separators at the boundary of this and
  /// the other path only.
  FilePath& operator/=(FilePath const& path);
  FilePath& operator/=(char const* path);

  FilePath operator/(FilePath const& path) const;
  FilePath operator/(char const* path) const;
};

//==============================================================================
// inline
//==============================================================================
inline
FilePath::FilePath(char const* path)
: Base(path)
{
  NormalizeDirSeparators();
}

//==============================================================================
inline
FilePath::FilePath(char const* path, size_t len)
: Base(path, len)
{}

//==============================================================================
inline
FilePath::FilePath(Base const& rhs)
: Base(rhs)
{}

//==============================================================================
inline
bool FilePath::StartsWith(char const* str, size_t strSize) const
{
  return strncmp(m_buffer, str, strSize) == 0;
}

//==============================================================================
inline
bool FilePath::StartsWith(char const* str) const
{
  return StartsWith(str, strlen(str));
}

//==============================================================================
inline
bool FilePath::StartsWith(FilePath const& other) const
{
  return StartsWith(other.m_buffer, other.m_size);
}

//==============================================================================
inline
FilePath& FilePath::operator=(char const* path)
{
  Base::operator=(path);
  NormalizeDirSeparators();
  return *this;
}

//==============================================================================
inline
FilePath& FilePath::operator/=(FilePath const& path)
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

//==============================================================================
inline
FilePath& FilePath::operator/=(char const* path)
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

//==============================================================================
inline
FilePath FilePath::operator/(FilePath const& path) const
{
  FilePath newPath(*this);
  return newPath /= path;
}

//==============================================================================
inline
FilePath FilePath::operator/(char const* path) const
{
  FilePath newPath(*this);
  return newPath /= path;
}

} // xr

#endif // XR_FILEPATH
