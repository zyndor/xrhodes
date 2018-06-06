//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/FileWriter.hpp"

namespace xr
{

//==============================================================================
FileWriter::~FileWriter()
{
  Close();
}

//==============================================================================
static char const* const kMode[] =
{
  "wb",
  "ab",
  "w",
  "a"
};

bool FileWriter::Open(FilePath const& path, Mode mode, bool text)
{
  Close();

  auto const offset = (2 * (text ? 1 : 0));
  auto handle = File::Open(path, kMode[static_cast<int>(mode) + offset]);
  m_handle = handle;
  return handle != nullptr;
}

//==============================================================================
bool FileWriter::Write(void const* buffer, size_t elemSize, size_t numElems)
{
  return File::Write(buffer, elemSize, numElems, m_handle) == numElems;
}

//==============================================================================
size_t FileWriter::GetSize() const
{
  return m_handle ? File::GetSize(m_handle) : 0;
}

//==============================================================================
void FileWriter::Close()
{
  if (m_handle)
  {
    File::Close(m_handle);
    m_handle = nullptr;
  }
}

} // XR
