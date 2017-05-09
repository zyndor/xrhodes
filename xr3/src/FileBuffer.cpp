//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <string>
#include <XR/FileBuffer.hpp>
#include <XR/debug.hpp>

namespace XR
{

//==============================================================================
FileBuffer::FileBuffer()
: m_handle(File::INVALID_HANDLE),
  m_flags(0),
  m_size(0),
  m_pData(0)
{}

//==============================================================================
FileBuffer::~FileBuffer()
{
  Close();
  Destroy();
}

//==============================================================================
bool  FileBuffer::Open(const char* pName, const char* pMode)
{
  XR_ASSERT(FileBuffer, pName != 0);
  XR_ASSERT(FileBuffer, pMode != 0);
  m_handle = File::Open(pName, pMode);
  bool  success(m_handle != File::INVALID_HANDLE);
  if(success)
  {
    m_flags = 0;
    if(strchr(pMode, 'r') != 0)
    {
      m_flags |= READ_F;
    }

    if(strchr(pMode, 'w') != 0)
    {
      m_flags |= WRITE_F;
    }

    if(strchr(pMode, 'b') != 0)
    {
      m_flags |= BINARY_F;
    }

    if(HasFlag(READ_F))
    {
      m_size = File::GetSize(m_handle);
      m_pData = new char[m_size + 1];
      success = File::Read(m_handle, m_size, 1, m_pData) == 1;
      m_pData[m_size] = '\0';
    }
  }
  return success;
}

//==============================================================================
bool  FileBuffer::Write(const void* pBuffer, size_t size, size_t numElems)
{
  return HasFlag(WRITE_F) &&
    File::Write(pBuffer, size, numElems, m_handle) == numElems;
}

//==============================================================================
void  FileBuffer::Close()
{
  if(m_handle != File::INVALID_HANDLE)
  {
    File::Close(m_handle);
    m_handle = File::INVALID_HANDLE;
  }
}

//==============================================================================
void  FileBuffer::Destroy()
{
  if(m_pData != 0)
  {
    delete[] m_pData;
    m_pData = 0;
  }
}

} // XR