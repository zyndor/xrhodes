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
#include <sys/stat.h>
#include <algorithm>
#include <fstream>

#ifdef XR_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define XR_ASSERT_HANDLE_VALID(h) XR_ASSERTMSG(File, h != nullptr, ("Invalid file handle."))

namespace XR
{

namespace
{

static struct
{
  bool init = false;
  File::System system;
} s_file;

// Abstraction of the actual operation on a concrete path and whether is applicable for
// read only locations.
struct FileOp
{
  // A file operation following XRhodes' logic in helping to resolve paths.
  // - if raw:// was explicitly specified, process the rest of the path as is. otherwise:
  // - strip leading slashes,
  // - unless ROM was explicitly specified, look in RAM,
  //   - If RAM was not explicitly specified, prepend path with it, if we can, otherwise warn and bail.
  // - if RAM didn't work out and ROM is applicable, look in ROM
  //   - If ROM was not explicitly specified, prepend path with it, if we can, otherwise warn and bail.
  bool Perform(FilePath const& path)
  {
    bool success = false;
    char const* cpath = path.c_str();
    // If explicit raw path, then only process the location and apply no further cleverness.
    if (path.StartsWith(File::kRawProto))
    {
      cpath += File::kRawProto.size();
      success = Process(cpath);
    }
    else
    {
      // strip leading slashes
      size_t size = path.size();
      while (cpath[0] == FilePath::kDirSeparator)
      {
        ++cpath;
        --size;
      }

      FilePath path = cpath;
      const bool explicitRom = path.StartsWith(File::GetRomPath());
      // unless ROM was explicitly specified, look in RAM first.
      if (!explicitRom)
      {
        bool doRam = true;
        if (!path.StartsWith(File::GetRamPath()))
        {
          doRam = File::GetRamPath().size() + size <= FilePath::kCapacity;
          if (doRam)
          {
            path = File::GetRamPath() + path;
          }
          else
          {
            XR_TRACE(File, ("Skipping RAM for '%s'; concatenated path length exceeds FilePath::kCapacity.", path.c_str()));
          }
        }

        if (doRam)
        {
          success = Process(path.c_str());
        }
      }

      // If RAM didn't work out, we can have a go with ROM - if applicable.
      if (!success && RomApplicable())
      {
        bool doRom = true;
        if (!explicitRom)
        {
          doRom = File::GetRomPath().size() + size <= FilePath::kCapacity;
          if (doRom)
          {
            path = File::GetRomPath() + path;
          }
          else
          {
            XR_TRACE(File, ("Skipping ROM for '%s'; concatenated path length exceeds FilePath::kCapacity.", path.c_str()));
          }
        }

        if (doRom)
        {
          success = Process(path.c_str());
        }
      }
    }
    return success;
  }

private:
  // NO virtual ~FileOp(), because not ever deleted, let alone via a pointer to base.
  virtual bool Process(char const* path) = 0;
  virtual bool RomApplicable() const { return false; }
};

}

//==============================================================================
FilePath const File::kRawProto("raw://");

//==============================================================================
void File::Init(System const& filesys)
{
  XR_ASSERTMSG(File, !s_file.init, ("Already initialised!"));
  s_file.system.ramPath = filesys.ramPath;
  s_file.system.ramPath.AppendDirSeparator();

  if (!filesys.romPath.empty())
  {
    s_file.system.romPath = filesys.romPath;
    s_file.system.romPath.AppendDirSeparator();
  }

  s_file.init = IsDir(s_file.system.ramPath, false) || MakeDirs(s_file.system.ramPath);
  XR_ASSERTMSG(File, s_file.init, ("Unable to access RAM path."));
}

//==============================================================================
void File::Exit()
{
  XR_ASSERTMSG(File, s_file.init, ("Not initialised!"));
  s_file.init = false;
}

//==============================================================================
FilePath const& File::GetRamPath()
{
  return s_file.system.ramPath;
}

//==============================================================================
FilePath const& File::GetRomPath()
{
  return s_file.system.romPath;
}

//==============================================================================
FilePath File::StripRoots(FilePath path)
{
  if (path.StartsWith(GetRamPath()))
  {
    path = path.data() + GetRamPath().size();
  }
  else if (path.StartsWith(GetRomPath()))
  {
    path = path.data() + GetRomPath().size();
  }
  return path;
}

//==============================================================================
bool File::CheckExists(FilePath const& name)
{
  return GetModifiedTime(name) != 0;
}

//==============================================================================
time_t File::GetModifiedTime(FilePath const& name)
{
  time_t modTime = 0;
  struct GetModifiedTimeOp : FileOp
  {
    struct stat statBuffer;
    time_t& modTime;

    GetModifiedTimeOp(time_t& outModTime)
    : modTime(outModTime)
    {}

    bool Process(char const* path)
    {
      bool result = stat(path, &statBuffer) == 0;
      if (result)
      {
#if defined(XR_PLATFORM_OSX) || defined(XR_PLATFORM_IOS)
#define XR_MTIME_OVERRIDE
#define st_mtime st_mtimespec.tv_sec
#endif
        modTime = statBuffer.st_mtime;
#ifdef XR_MTIME_OVERRIDE
#undef XR_MTIME_OVERRIDE
#undef st_mtime
#endif
      }
      return result;
    }

    bool RomApplicable() const
    {
      return true;
    }
  } op(modTime);

  op.Perform(name);
  return modTime;
}

//==============================================================================
File::Handle File::Open(FilePath const& name, const char* mode)
{
  FILE* file = nullptr;
  struct OpenOp : FileOp
  {
    char const* const mode;
    FILE*& file;

    OpenOp(char const* mode_, FILE*& outFile)
    : mode(mode_),
      file(outFile)
    {}

    bool Process(char const* path)
    {
      file = fopen(path, mode);
      return file != nullptr;
    }

    bool RomApplicable() const
    {
      return strchr(mode, 'r') && !strchr(mode, '+');
    }
  } op(mode, file);

  op.Perform(name);
  return file;
}

//==============================================================================
size_t File::GetSize(Handle hFile)
{
  XR_ASSERT_HANDLE_VALID(hFile);
  const size_t offset = Tell(hFile);
  size_t result = 0;
  if (Seek(hFile, 0, SeekFrom::End))
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
bool File::IsDir(FilePath const& path, bool includeRom)
{
  bool isDir = false;
  struct Proc : FileOp
  {
    struct stat statBuffer;
    bool& isDir;
    bool includeRom;

    Proc(bool& outIsDir, bool includeRom_)
    : isDir(outIsDir),
      includeRom(includeRom_)
    {}

    bool Process(char const* path)
    {
      bool result = stat(path, &statBuffer) == 0;
      if (result)
      {
        isDir = IsFullMask(statBuffer.st_mode, S_IFDIR);
      }
      return isDir;
    }

    bool RomApplicable() const
    {
      return includeRom;
    }
  } op(isDir, includeRom);

  return op.Perform(path);
}

//==============================================================================
bool File::MakeDir(FilePath const& path)
{
  struct : FileOp
  {
    bool Process(char const* path)
    {
#ifdef XR_PLATFORM_WINDOWS
      return _mkdir(path) == 0;
#else
      return mkdir(path, S_IRWXU) == 0;
#endif
    }
  } op;

  return op.Perform(path);
}

//==============================================================================
bool File::MakeDirs(FilePath const& path)
{
  bool success = true;
  char const* start = path.c_str();
  FilePath dir;
  while (auto end = strchr(start, FilePath::kDirSeparator))
  {
    size_t size = end - start;
    if (size > 0)
    {
      dir.append(start, end - start);
      dir.AppendDirSeparator();
      if (!IsDir(dir, false) && !MakeDir(dir))
      {
        success = false;
        break;
      }
    }
    start = end + 1;
  }
  return success;
}

} // XR
