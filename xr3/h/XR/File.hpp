//
// Nuclear Heart Games
// XRhodes
//
// File.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    08/10/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_FILE_HPP
#define XR_FILE_HPP

#include "fundamentals.hpp"
#include "types.hpp"

namespace XR
{

//==============================================================================
class File
{
  XR_NONOBJECT_DECL(File)

public:
  // types
  enum
  {
    INVALID_HANDLE = -1
  };
  
  enum  SecureError
  {
    SERR_NONE,
    SERR_PARAM,
    SERR_NOT_FOUND,
    SERR_INSUFFICIENT,
    SERR_DEVICE,
    SERR_CORRUPT,
    SERR_UNKNOWN
  };
  
  enum  Error
  {
    ERR_NONE,
    ERR_PARAM,
    ERR_NOT_FOUND,
    ERR_DEVICE,
    ERR_OS_MEMORY,
    ERR_IO,
    ERR_UNKNOWN,
    
    ERR_INVALID_MODE,
    ERR_EXISTS,
    ERR_NOT_EMPTY,
    ERR_ACCESS,
    ERR_EOF
  };
  
  enum  SeekFrom
  {
    SF_START,
    SF_CURRENT,
    SF_END
  };
  
  // static
  static void         Init();
  static void         Exit();
  
  static bool         SecureSave(void* pBuffer, uint16 size);
  static bool         SecureLoad(void* pBuffer, uint16 size);
  static SecureError  SecureGetError();
  static const char*  SecureGetErrorString();

  static bool         CheckExists(const char* pName);
  static int          Open(const char* pName, const char* pMode);
  static size_t       GetSize(int hFile);
  static const char*  GetName(int hFile);
  static size_t       Read(int hFile, size_t elemSize, size_t numElems, void* parBuffer);
  static char*        ReadLine(int hFile, size_t bufferSize, char* parBuffer);
  static size_t       Write(const void* parBuffer, size_t elemSize, size_t numElems, int hFile);
  static size_t       Tell(int hFile);
  static bool         Seek(int hFile, size_t offset, SeekFrom sf);
  static void         Close(int hFile);
  
  static Error        GetError();
  static const char*  GetErrorString();
};

} // XR

#endif  //XR_FILE_HPP