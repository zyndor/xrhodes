#include  <algorithm>
#include  <fstream>
#include  "File.hpp"
#include  "HardString.hpp"

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
  for(int i = 0; i < kMaxFileHandles; ++i)
  {
    Close(i);
  }
}

//==============================================================================
bool File::SecureSave( void* pBuffer, uint16 size )
{
  return false;
}

//==============================================================================
bool File::SecureLoad( void* pBuffer, uint16 size )
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
bool File::CheckExists( const char* pName )
{
  FILE* pFile(fopen(pName, "r"));
  bool  result = pFile != 0;
  if(result)
  {
    fclose(pFile);
  }
  return result;
}

//==============================================================================
int File::Open( const char* pName, const char* pMode )
{
  int hFile(INVALID_HANDLE);
  for(int i = 0; i < kMaxFileHandles; ++i)
  {
    int hTemp((s_fileImpl.nextHandle + i) % kMaxFileHandles);
    if(s_fileImpl.arpFile[hTemp] == 0)
    {
      hFile = hTemp;
      break;
    }
  }
  
  if(hFile != INVALID_HANDLE)
  {
    FILE*&  pFile = s_fileImpl.arpFile[hFile];
    
    pFile = fopen(pName, pMode);
    if(pFile != 0)
    {
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
uint64 File::GetSize( int hFile )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  
  uint64 offset(ftell(s_fileImpl.arpFile[hFile]));
  uint64 result(0);
  if(0 == fseek(s_fileImpl.arpFile[hFile], 0, SEEK_END))
  {
    result = ftell(s_fileImpl.arpFile[hFile]);
  }
  
  fseek(s_fileImpl.arpFile[hFile], offset, SEEK_SET);

  return result;
}

//==============================================================================
const char* File::GetName( int hFile )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return s_fileImpl.arFilename[hFile].c_str();
}

//==============================================================================
uint32 File::Read( int elemSize, int numElems, int hFile, void* parBuffer )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return fread(parBuffer, elemSize, numElems, s_fileImpl.arpFile[hFile]);
}

//==============================================================================
char* File::ReadString( int numBytes, int hFile, char* parBuffer )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  for(int i = 0; i < numBytes; ++i)
  {
    int result = fread(parBuffer + i, 1, 1, s_fileImpl.arpFile[hFile]);
    if(result < 1 || parBuffer[i] == '\r' || parBuffer[i] == '\n')
    {
      break;
    }
  }

  return parBuffer;
}

//==============================================================================
uint32 File::Write(const void* parBuffer, int elemSize, int numElems, int hFile )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return fwrite(parBuffer, elemSize, numElems, s_fileImpl.arpFile[hFile]);
}

//==============================================================================
void File::Close( int hFile )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  FILE*&  pFile = s_fileImpl.arpFile[hFile];
  if(pFile != 0)
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
    karErrorMappings + XR_ARR_SIZE(karErrorMappings),
    error) - karErrorMappings);
}

//==============================================================================
uint64  File::Tell( int hFile )
{
  XR_ASSERT(File, hFile >= 0);
  XR_ASSERT(File, hFile < kMaxFileHandles);
  XR_ASSERT(File, s_fileImpl.arpFile[hFile] != 0);
  return ftell(s_fileImpl.arpFile[hFile]);
}

//==============================================================================
static const int  karSeekOriginMappings[] =
{
  SEEK_SET,
  SEEK_CUR,
  SEEK_END,
};

bool File::Seek( int hFile, int offset, SeekFrom sf )
{
  return fseek(s_fileImpl.arpFile[hFile], offset, karSeekOriginMappings[sf]);
}

//==============================================================================
const char* File::GetErrorString()
{
  return 0; //s3eFileGetErrorString();
}

} // XR