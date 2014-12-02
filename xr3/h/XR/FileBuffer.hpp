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
#if !defined XR_FILEBUFFER_HPP
#define XR_FILEBUFFER_HPP

#include "File.hpp"

namespace XR
{

//==============================================================================
class FileBuffer
{
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
  bool        HasFlag(uint32 flags) const;

  int         GetSize() const;
  const char* GetData() const;

  bool        Write(const void* pBuffer, int size, int numElems);
  void        Close();

  void        Destroy();

private:
  // data
  int     m_handle;
  uint32  m_flags;
  int     m_size;
  char*   m_pData;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  FileBuffer::HasFlag(uint32 flags) const
{
  return (m_flags & flags) == flags;
}

//==============================================================================
inline
int FileBuffer::GetSize() const
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