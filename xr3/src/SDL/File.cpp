//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/File.hpp>
#include <XR/stringutils.hpp>
#include <XR/utils.hpp>
#include <XR/debug.hpp>
#include <algorithm>
#include <fstream>

namespace XR
{

static struct
{
  bool init = false;
  File::System system;
  size_t ramPathSize;
  size_t romPathSize;
} s_file;

#define XR_ASSERT_HANDLE_VALID(h) XR_ASSERTMSG(File, h != nullptr, ("Invalid file handle."))

//==============================================================================
void AppendSlash(File::Path& path)
{
  char* writep = path.data() + path.size() - 1;
  if (*writep != '/')
  {
    path += "/";
  }
}

//==============================================================================
void File::Init(System const& filesys)
{
  XR_ASSERTMSG(File, !s_file.init, ("Already initialised!"));
  size_t length;
  if (!filesys.ramPath.empty())
  {
    Replace(filesys.ramPath.c_str(), "\\", "/", s_file.system.ramPath.capacity(),
      s_file.system.ramPath.data(), length);
    AppendSlash(s_file.system.ramPath);
  }
  s_file.ramPathSize = s_file.system.ramPath.size();

  if (!filesys.romPath.empty())
  {
    Replace(filesys.romPath.c_str(), "\\", "/", s_file.system.romPath.capacity(),
      s_file.system.romPath.data(), length);
    AppendSlash(s_file.system.romPath);
  }
  s_file.romPathSize = s_file.system.romPath.size();

  s_file.init = true;
}

//==============================================================================
void File::Exit()
{
  XR_ASSERTMSG(File, s_file.init, ("Not initialised!"));
  s_file.init = false;
}

//==============================================================================
File::Path const& File::GetRamPath()
{
  return s_file.system.ramPath;
}

//==============================================================================
File::Path const& File::GetRomPath()
{
  return s_file.system.romPath;
}

//==============================================================================
bool File::CheckExists(Path const& name)
{
  Handle h = Open(name, "rb");
  if (h)
  {
    Close(h);
  }
  return h != nullptr;
}

//==============================================================================
File::Handle File::Open(Path const& name, const char* mode)
{
  FILE*  file = fopen(name.c_str(), mode);

  if (!file)
  {
    // Skip any heading slashes.
    char const* nameChars = name.c_str();
    if (nameChars[0] == '/')
    {
      ++nameChars;
    }
    const size_t nameLen = strlen(nameChars);

    // Try ramPath.
    if (!GetRamPath().empty() &&
      nameLen + s_file.ramPathSize <= Path::kCapacity &&
      strncmp(nameChars, GetRamPath().c_str(), s_file.ramPathSize) != 0)
    {
      Path path = s_file.system.ramPath;
      path += nameChars;

      file = fopen(path.c_str(), mode);
    }

    // If we still haven't found our file in ramPath, and we only want to read,
    // then we may try romPath.
    if (!file && !GetRomPath().empty() &&
      nameLen + s_file.romPathSize <= Path::kCapacity &&
      strncmp(nameChars, GetRomPath().c_str(), s_file.romPathSize) != 0 &&
      strchr(mode, 'r') && !strchr(mode, 'w'))
    {
      Path path = s_file.system.romPath;
      path += nameChars;

      file = fopen(path.c_str(), mode);
    }
  }
  return file;
}

//===============-===============================================================
size_t File::GetSize(Handle hFile)
{
  XR_ASSERT_HANDLE_VALID(hFile);
  const size_t offset = Tell(hFile);
  size_t result = 0;
  if (0 == Seek(hFile, 0, SeekFrom::End))
  {
    result = Tell(hFile);
    Seek(hFile, offset, SeekFrom::Start);
  }
  
  return result;
}

//==============================================================================
size_t File::Read(Handle hFile, size_t elemSize, size_t numElems, void* parBuffer)
{
  XR_ASSERT_HANDLE_VALID(hFile);
  FILE* f = static_cast<FILE*>(hFile);
  return fread(parBuffer, elemSize, numElems, f);
}

//==============================================================================
size_t File::Write(void const* parBuffer, size_t elemSize, size_t numElems, Handle hFile)
{
  XR_ASSERT(File, hFile);
  FILE* f = static_cast<FILE*>(hFile);
  return fwrite(parBuffer, elemSize, numElems, f);
}

//==============================================================================
void File::Close(Handle hFile)
{
  FILE* f = static_cast<FILE*>(hFile);
  if (f)
  {
    fclose(f);
  }
}

//==============================================================================
size_t File::Tell(Handle hFile)
{
  XR_ASSERT_HANDLE_VALID(hFile);
  FILE* f = static_cast<FILE*>(hFile);
  const size_t tell = ftell(f);
  return tell;
}

//==============================================================================
static const int  karSeekOriginMappings[] =
{
  SEEK_SET,
  SEEK_CUR,
  SEEK_END,
};

bool File::Seek(Handle hFile, size_t offset, SeekFrom sf)
{
  XR_ASSERT_HANDLE_VALID(hFile);
  FILE* f = static_cast<FILE*>(hFile);
  return fseek(f, offset, karSeekOriginMappings[static_cast<int>(sf)]) == 0;
}

} // XR
