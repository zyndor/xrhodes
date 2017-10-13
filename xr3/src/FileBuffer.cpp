//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <string>
#include "XR/FileBuffer.hpp"
#include "XR/debug.hpp"

namespace XR
{

//==============================================================================
FileBuffer::~FileBuffer()
{
  Destroy();
  Close();
}

//==============================================================================
bool  FileBuffer::Open(FilePath const& path, bool text)
{
  Destroy();
  Close();

  m_handle = File::Open(path, text ? "r" : "rb");
  bool success = m_handle != nullptr;
  if (success)
  {
    m_size = File::GetSize(m_handle);
    m_data = new uint8_t[m_size];
    success = File::Read(m_handle, m_size, 1, m_data) == 1;
  }
  return success;
}

//==============================================================================
uint8_t* FileBuffer::DisownData()
{
  auto data = m_data;
  m_data = nullptr;
  m_size = 0;
  return data;
}

//==============================================================================
void  FileBuffer::Close()
{
  if (m_handle)
  {
    File::Close(m_handle);
    m_handle = nullptr;
  }
}

//==============================================================================
void  FileBuffer::Destroy()
{
  if (m_data)
  {
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
  }
}

} // XR
