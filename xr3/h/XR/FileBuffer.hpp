//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FILEBUFFER_HPP
#define XR_FILEBUFFER_HPP

#include "File.hpp"
#include <XR/fundamentals.hpp>

namespace XR
{

//==============================================================================
class FileBuffer
{
	XR_NONCOPY_DECL(FileBuffer)

public:
  enum  Flags
  {
    READ_F = 0x01,
    WRITE_F = 0x02,
    BINARY_F = 0x04
  };

  // structors
  FileBuffer();
  ~FileBuffer();

  // general
  bool        Open(const char* pName, const char* pMode);
  bool        HasFlag(uint32_t flags) const;

  size_t      GetSize() const;
  const char* GetData() const;

  bool        Write(const void* pBuffer, size_t size, size_t numElems);
  void        Close();

  void        Destroy();

private:
  // data
  int       m_handle;
  uint32_t  m_flags;
  size_t    m_size;
  char*     m_pData;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  FileBuffer::HasFlag(uint32_t flags) const
{
  return (m_flags & flags) == flags;
}

//==============================================================================
inline
size_t FileBuffer::GetSize() const
{
  return m_size;
}

//==============================================================================
inline
const char* FileBuffer::GetData() const
{
  return m_pData;
}

} // XR

#endif  //XR_FILEBUFFER_HPP