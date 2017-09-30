//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FILE_HPP
#define XR_FILE_HPP

#include <XR/FilePath.hpp>
#include <XR/fundamentals.hpp>
#include <cstdint>

namespace XR
{

//==============================================================================
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
  static void   Exit();

  static FilePath const& GetRamPath();
  static FilePath const& GetRomPath();

  static FilePath StripRoots(FilePath path);

  static bool   CheckExists(FilePath const& path);
  static time_t GetModifiedTime(FilePath const& path);

  ///@brief Attempts to open a file in the given @a mode (standard fopen flags
  /// apply) and return a handle to it. If a file in the given path was not 
  /// found, then attempts will be made to locate the file in the ram and rom
  /// paths (given to Init()) by prepending @a name with them if possible
  /// (i.e. within the capacity of Path), in this order.
  /// This behaviour may be suppressed by prefixing the path with kRawPath.
  static Handle Open(FilePath const& path, const char* mode);

  static size_t GetSize(Handle hFile);

  ///@brief Number of elements actually written.
  static size_t Read(Handle hFile, size_t elemSize, size_t numElems, void* parBuffer);

  ///@brief Number of elements actually written.
  static size_t Write(const void* parBuffer, size_t elemSize, size_t numElems, Handle hFile);

  static size_t Tell(Handle hFile);

  ///@brief Repositions the position indicator for the given file handle.
  ///@return Boolean to indicate the success of the operation. Note how this is
  /// contrary to fseek(), which returns 0 on success.
  static bool   Seek(Handle hFile, size_t offset, SeekFrom sf);

  static void   Close(Handle hFile);

  ///@brief Checks if the given @a path is a directory. See the class docs
  /// about the application of RAM and ROM paths. ROM is only checked if
  /// @a includeRom is set.
  ///@return Whether the path is accessible, exists and is a directory.
  static bool   IsDir(FilePath const& path, bool includeRom);

  ///@brief Makes a directory, if it doesn't exist, in ramPath. A raw path may
  /// be used instead by prefixing @a path with kRawPath.
  ///@return Whether the operation was successful.
  ///@note All of @a path's parent directories must exist.
  static bool   MakeDir(FilePath const& path);

  ///@brief Makes a directory (and each parent as necessary), if it doesn't
  /// exist, in ramPath. A raw path may be used instead by prefixing @a path
  /// with kRawPath.
  ///@return Whether the operation was successful.
  static bool   MakeDirs(FilePath const& path);
};

} // XR

#endif  //XR_FILE_HPP
