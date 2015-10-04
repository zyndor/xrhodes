#include <algorithm>
#include <s3eSecureStorage.h>
#include <s3eFile.h>
#include "File.hpp"
#include "HardString.hpp"

namespace XR
{

//==============================================================================
typedef HardString<S3E_FILE_MAX_PATH>  Filename;

struct FileImpl
{
  int           secureError;
  s3eFile*      arFile[S3E_FILE_MAX_HANDLES];
  Filename      arFilename[S3E_FILE_MAX_HANDLES];
  int           nextHandle;
};

//==============================================================================
static FileImpl*  s_pFile(0);

//==============================================================================
void File::Init()
{
  XR_ASSERTMSG(File, s_pFile == 0, ("Already initialised!"));
  s_pFile = new FileImpl;

  memset(s_pFile, 0x00, sizeof(FileImpl));
}

//==============================================================================
void File::Exit()
{
  XR_ASSERTMSG(File, s_pFile != 0, ("Not initialised!"));
  for(int i = 0; i < S3E_FILE_MAX_HANDLES; ++i)
  {
    Close(i);
  }

  delete s_pFile;
  s_pFile = 0;
}

//==============================================================================
bool File::SecureSave(void* pBuffer, uint16 size)
{
  return s3eSecureStoragePut(pBuffer, size) == S3E_RESULT_SUCCESS;
}

//==============================================================================
bool File::SecureLoad(void* pBuffer, uint16 size)
{
  return s3eSecureStorageGet(pBuffer, size) == S3E_RESULT_SUCCESS;
}

//==============================================================================
static const int  karSecureErrorMappings[] =
{
  S3E_SECURESTORAGE_ERR_NONE,
  S3E_SECURESTORAGE_ERR_PARAM,
  S3E_SECURESTORAGE_ERR_NOT_FOUND,
  S3E_SECURESTORAGE_ERR_INSUFF,
  S3E_SECURESTORAGE_ERR_DEVICE,
  S3E_SECURESTORAGE_ERR_CORRUPT,
};

File::SecureError File::SecureGetError()
{
  return static_cast<SecureError>(std::find(karSecureErrorMappings,
    karSecureErrorMappings + XR_ARR_SIZE(karSecureErrorMappings),
    s3eSecureStorageGetError()) - karSecureErrorMappings);
}

//==============================================================================
const char* File::SecureGetErrorString()
{
  return s3eSecureStorageGetErrorString();
}

//==============================================================================
bool File::CheckExists(const char* pName)
{
  return s3eFileCheckExists(pName) != 0;
}

//==============================================================================
int File::Open(const char* pName, const char* pMode)
{
  int hFile(INVALID_HANDLE);
  for(int i = 0; i < S3E_FILE_MAX_HANDLES; ++i)
  {
    s3eFile* pFile(0);
    int hTemp((s_pFile->nextHandle + i) % S3E_FILE_MAX_HANDLES);
    if(s_pFile->arFile[hTemp] == 0)
    {
      hFile = hTemp;
      break;
    }
  }
  
  if(hFile != INVALID_HANDLE)
  {
    s3eFile*&  pFile = s_pFile->arFile[hFile];
    
    pFile = s3eFileOpen(pName, pMode);
    if(pFile != 0)
    {
      s_pFile->arFilename[hFile] = pName;
      s_pFile->nextHandle = hFile + 1;
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
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s3eFileGetSize(s_pFile->arFile[hFile]);
}

//==============================================================================
const char* File::GetName(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s_pFile->arFilename[hFile].c_str();
}

//==============================================================================
size_t File::Read(int hFile, size_t elemSize, size_t numElems, void* parBuffer)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s3eFileRead(parBuffer, elemSize, numElems, s_pFile->arFile[hFile]);
}

//==============================================================================
char* File::ReadLine(int hFile, size_t bufferSize, char* parBuffer)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s3eFileReadString(parBuffer, bufferSize, s_pFile->arFile[hFile]);
}

//==============================================================================
size_t File::Write(const void* parBuffer, size_t elemSize, size_t numElems, int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s3eFileWrite(parBuffer, elemSize, numElems, s_pFile->arFile[hFile]);
}

//==============================================================================
void File::Close(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  s3eFile*&  pFile = s_pFile->arFile[hFile];
  if(pFile != 0)
  {
    s3eFileClose(pFile);
    pFile = 0;
    s_pFile->nextHandle = hFile;
  }
}

//==============================================================================
static const int  karErrorMappings[] =
{
  S3E_FILE_ERR_NONE,
  S3E_FILE_ERR_PARAM,
  S3E_FILE_ERR_NOT_FOUND,
  S3E_FILE_ERR_DEVICE,
  S3E_FILE_ERR_MEM,
  S3E_FILE_ERR_DATA,
  S3E_FILE_ERR_GENERIC,
  S3E_FILE_ERR_INVALID_MODE,
  S3E_FILE_ERR_EXISTS,
  S3E_FILE_ERR_NOT_EMPTY,
  S3E_FILE_ERR_ACCESS,
  S3E_FILE_ERR_EOF
};

File::Error File::GetError()
{
  int error(s3eFileGetError());
  return static_cast<Error>(std::find(karErrorMappings,
    karErrorMappings + XR_ARR_SIZE(karErrorMappings),
    error) - karErrorMappings);
}

//==============================================================================
size_t File::Tell(int hFile)
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < S3E_FILE_MAX_HANDLES);
  XR_ASSERT(File, s_pFile->arFile[hFile] != 0);
  return s3eFileTell(s_pFile->arFile[hFile]);
}

//==============================================================================
static const s3eFileSeekOrigin  karSeekOriginMappings[] =
{
  S3E_FILESEEK_SET,
  S3E_FILESEEK_CUR,
  S3E_FILESEEK_END,
};

bool File::Seek(int hFile, size_t offset, SeekFrom sf)
{
  return s3eFileSeek(s_pFile->arFile[hFile], offset,
    karSeekOriginMappings[sf]) == S3E_RESULT_SUCCESS;
}

//==============================================================================
const char* File::GetErrorString()
{
  return s3eFileGetErrorString();
}

} // XR