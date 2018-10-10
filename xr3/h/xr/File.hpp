#ifndef XR_FILE_HPP
#define XR_FILE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "xr/FilePath.hpp"
#include "xr/fundamentals.hpp"
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Abstraction for common file operations on the platform. Offers support
/// for a RAM and a ROM location which it's initialised with. ROM is optional.
/// For operations that require a path, the following logic is used:
/// 1, if the path is prefixed with "raw://", the path is used as-is.
/// 2, otherwise, if ROM was not explicitly specified, RAM is cheked for the
///   path, if only RAM is either explicitly specified or the path is prefixable
///   with it.
/// 3, if the operation could not be performed on the RAMified path and ROM is
///   applicable (which it isn't with e.g. making a directory), then ROM is
///   checked for the path, if only ROM is either explicitly specified or the
///   path is prefixeble with it.
class File
{
  XR_NONOBJECT_DECL(File)

public:
  // types
  using Handle = void*;

  enum class SeekFrom
  {
    Start,
    Current,
    End
  };

  struct System
  {
    FilePath ramPath;
    FilePath romPath;
  };

  // static
  static FilePath const kRawProto;

  ///@brief Initialises the file system with the given paths. Backslashes will
  /// be converted to slashes and for the paths that don't end in a slash, one
  /// will be appended.
  static void   Init(System const& filesys);
  static void   Shutdown();

  static FilePath const& GetRamPath();
  static FilePath const& GetRomPath();

  ///@brief If path starts with either the RAM or ROM paths, it'll remove it
  /// and return the processed path.
  static FilePath StripRoots(FilePath path);

  ///@brief Checks for the existence of a file on the given @a path. See the
  /// class docs about the application of RAM and ROM paths.
  static bool   CheckExists(FilePath const& path);

  ///@return Timestamp of last modification of the given file or 0 if the
  /// file doesn't exist. See the class docs about the application
  /// of RAM and ROM paths.
  static time_t GetModifiedTime(FilePath const& path);

  ///@brief Attempts to open a file in the given @a mode (standard fopen flags
  /// apply) and return a handle to it. See the class docs about the application
  /// of RAM and ROM paths.
  ///@return Handle to the file opened, nullptr if couldn't be opened.
  static Handle Open(FilePath const& path, const char* mode);

  ///@return The size of the given file, in bytes.
  static size_t GetSize(Handle hFile);

  ///@brief Reads from the file into the given buffer, @a numElems elements of
  /// size @a elemSize.
  ///@return Number of elements actually read.
  static size_t Read(Handle hFile, size_t elemSize, size_t numElems, void* parBuffer);

  ///@brief Writes to the file from the given buffer, @a numElems elements of
  /// size @a elemSize.
  ///@return Number of elements actually written.
  static size_t Write(const void* parBuffer, size_t elemSize, size_t numElems, Handle hFile);

  ///@return The byte offset of the current read/write position from the start
  /// of the file.
  static size_t Tell(Handle hFile);

  ///@brief Repositions the position indicator for the given file handle.
  ///@return Boolean to indicate the success of the operation.
  ///@note This is different from fseek(), which returns 0 on success.
  static bool Seek(Handle hFile, size_t offset, SeekFrom sf);

  ///@brief Closes and invalidates the file handle.
  static void Close(Handle hFile);

  ///@brief Deletes the file in ramPath, at the given @a path. A raw path may
  /// be used instead by prefixing path with kRawProto.
  ///@return Whether the operation was successful.
  static bool Delete(FilePath const& path);

  ///@brief Checks if the given @a path is a directory. See the class docs
  /// about the application of RAM and ROM paths. ROM is only checked if
  /// @a includeRom is set.
  ///@return Whether the path exists, is accessible and is a directory.
  static bool IsDir(FilePath const& path, bool includeRom);

  ///@brief Makes a directory, if it doesn't exist, in ramPath. A raw path may
  /// be used instead by prefixing @a path with kRawProto.
  ///@return Whether the operation was successful.
  ///@note All of @a path's parent directories must exist.
  static bool MakeDir(FilePath const& path);

  ///@brief Makes a directory (and each parent as necessary), if it doesn't
  /// exist, in ramPath. A raw path may be used instead by prefixing @a path
  /// with kRawProto.
  ///@return Whether the operation was successful.
  static bool MakeDirs(FilePath const& path);
};

} // XR

#endif  //XR_FILE_HPP
