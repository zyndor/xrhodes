#include  "RenderStream.hpp"

namespace XR
{

//==============================================================================
bool  RenderStream::ReadFile(int hFile)
{
  Destroy();

  Format  fmt(RenderStream::F_INVALID);
  int     capacity(0);
  bool    success(File::Read(hFile, sizeof(fmt), 1, &fmt) == 1 &&
            File::Read(hFile, sizeof(capacity), 1, &capacity) == 1);
  if(success)
  {
    Create(fmt, capacity);
    success =
      File::Read(hFile, CalculateByteSize(fmt, capacity), 1, GetData()) == 1;
  }

  return success;
}

//==============================================================================
bool  RenderStream::WriteFile(int hFile) const
{
  Format  fmt(GetFormat());
  int     capacity(GetCapacity());
  bool    success(File::Write(&fmt, sizeof(fmt), 1, hFile) == 1 &&
            File::Write(&capacity, sizeof(capacity), 1, hFile) == 1);
  if(success && capacity > 0)
  {
    success =
      File::Write(GetData(), CalculateByteSize(fmt, capacity), 1, hFile) == 1;
  }

  return success;
}

} // XR