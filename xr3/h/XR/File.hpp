//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FILE_HPP
#define XR_FILE_HPP

#include <XR/HardString.hpp>
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

  using Path = HardString<256>;

  struct System
  {
    Path ramPath;
    Path romPath;
  };
  
  // static
  ///@brief Initialises the file system with the given paths. Backslashes will
  /// be converted to slashes and for the paths that don't end in a slash, one
  /// will be appended.
  static void   Init(System const& filesys);
  static void   Exit();

  static Path const& GetRamPath();
  static Path const& GetRomPath();

  static bool   CheckExists(Path const& name);

  ///@brief Attempts to open a file in the given @a mode (standard fopen flags
  /// apply) and return a handle to it. If a file in the given path was not found,
  /// then attempts will be made to append the ram and rom paths (given to Init())
  /// to @a name and open the file there, in this order.
  static Handle Open(Path const& name, const char* mode);

  static size_t GetSize(Handle hFile);
  static size_t Read(Handle hFile, size_t elemSize, size_t numElems, void* parBuffer);
  static size_t Write(const void* parBuffer, size_t elemSize, size_t numElems, Handle hFile);
  static size_t Tell(Handle hFile);
  static bool   Seek(Handle hFile, size_t offset, SeekFrom sf);
  static void   Close(Handle hFile);
};

} // XR

#endif  //XR_FILE_HPP
