//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/File.hpp>
#include <XR/HardString.hpp>
#include <XR/utils.hpp>
#include <algorithm>
#include <fstream>

namespace XR
{

//==============================================================================
typedef HardString<128>  Filename;

//==============================================================================
static const int  kMaxFileHandles = 32;

//==============================================================================
static struct 
{
  int           secureError;
  FILE*         arpFile[kMaxFileHandles];
  Filename      arFilename[kMaxFileHandles];
  int           nextHandle;
} s_fileImpl;

//==============================================================================
void File::Init()
{
  memset(&s_fileImpl, 0x00, sizeof(s_fileImpl));
}

//==============================================================================
void File::Exit()
{
  for (int i = 0; i < kMaxFileHandles; ++i)
  {
    Close(i);
  }
}

//==============================================================================
bool File::SecureSave(void* pBuffer, uint16_t size)
{
  return false;
}

//==============================================================================
bool File::SecureLoad(void* pBuffer, uint16_t size)
{
  return false;
}

//==============================================================================
static const int  karSecureErrorMappings[] =
{
  0  // stub
};

File::SecureError File::SecureGetError()
{
  return SERR_NONE; // stub
}

//==============================================================================
const char* File::SecureGetErrorString()
{
  return ""; // stub
}

//==============================================================================
bool File::CheckExists(const char* pName)
{
  FILE* pFile(fopen(pName, "r"));
  bool  result = pFile != 0;
  if (result)
  {
    fclose(pFile);
  }
  return result;
}

//==============================================================================
int File::Open(const char* pName, const char* pMode)
{
  int hFile(INVALID_HANDLE);
  for (int i = 0; i < kMaxFileHandles; ++i)
  {
    int hTemp((s_fileImpl.nextHandle + i) % kMaxFileHandles);
    if (s_fileImpl.arpFile[hTemp] == 0)
    {
      hFile = hTemp;
      break;
    }
  }
  
  if (hFile != INVALID_HANDLE)
  {
    FILE*  pFile = fopen(pName, pMode);
    if (pFile != 0)
    {
      s_fileImpl.arpFile[hFile] = pFile;
      s_fileImpl.arFilename[hFile] = pName;
      s_fileImpl.nextHandle = hFile + 1;
    }
    else
    {
      hFile = INVALID_HANDLE;
    }
  }
  return hFile;
}

//==============================================================================
size_t File::GetSize(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  
  size_t offset(ftell(s_fileImpl.arpFile[hFile]));
  size_t result(0);
  if (0 == fseek(s_fileImpl.arpFile[hFile], 0, SEEK_END))
  {
    result = ftell(s_fileImpl.arpFile[hFile]);
  }
  
  fseek(s_fileImpl.arpFile[hFile], offset, SEEK_SET);

  return result;
}

//==============================================================================
const char* File::GetName(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return s_fileImpl.arFilename[hFile].c_str();
}

//==============================================================================
size_t File::Read(int hFile, size_t elemSize, size_t numElems, void* parBuffer)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return fread(parBuffer, elemSize, numElems, s_fileImpl.arpFile[hFile]);
}

//==============================================================================
char* File::ReadLine(int hFile, size_t bufferSize, char* parBuffer)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  XR_ASSERT(File, parBuffer != 0);
  --bufferSize;
  int result(0);
  char* pWrite(parBuffer);
  for (size_t i = 0; i < bufferSize; ++i)
  {
    result = fread(pWrite, 1, 1, s_fileImpl.arpFile[hFile]);
    char  c(*pWrite);
    bool  success(result == 1);
    if(success)
    {
      ++pWrite;
    }
    
    if (!success || c == '\r' || c == '\n')
    {
      break;
    }
  }
  
  *pWrite = '\0';

  return parBuffer;
}

//==============================================================================
size_t File::Write(const void* parBuffer, size_t elemSize, size_t numElems, int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return fwrite(parBuffer, elemSize, numElems, s_fileImpl.arpFile[hFile]);
}

//==============================================================================
void File::Close(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  FILE*&  pFile = s_fileImpl.arpFile[hFile];
  if (pFile != 0)
  {
    fclose(pFile);
    pFile = 0;
    s_fileImpl.nextHandle = hFile;
  }
}

//==============================================================================
static const int  karErrorMappings[] =
{
  0
};

File::Error File::GetError()
{
  int error(0);
  return static_cast<Error>(std::find(karErrorMappings,
    karErrorMappings + XR_ARRAY_SIZE(karErrorMappings),
    error) - karErrorMappings);
}

//==============================================================================
size_t  File::Tell(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  
  size_t  tell(ftell(s_fileImpl.arpFile[hFile]));
  return tell;
}

//==============================================================================
static const int  karSeekOriginMappings[] =
{
  SEEK_SET,
  SEEK_CUR,
  SEEK_END,
};

bool File::Seek(int hFile, size_t offset, SeekFrom sf)
{
  return fseek(s_fileImpl.arpFile[hFile], offset, karSeekOriginMappings[sf]) == 0;
}

//==============================================================================
const char* File::GetErrorString()
{
  return 0; //s3eFileGetErrorString();
}

} // XR