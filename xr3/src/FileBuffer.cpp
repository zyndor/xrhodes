//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <string>
#include "xr/FileBuffer.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
FileBuffer::FileBuffer(FileBuffer&& rhs)
: m_handle(rhs.m_handle),
  m_size(rhs.m_size),
  m_data(rhs.m_data)
{
  rhs.m_handle = nullptr;
  rhs.m_data = nullptr;
  rhs.m_size = 0;
}

//==============================================================================
FileBuffer::~FileBuffer()
{
  Destroy();
  Close();
}

//==============================================================================
bool  FileBuffer::Open(FilePath const& path)
{
  Destroy();
  Close();

  m_handle = File::Open(path, "rb");
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

//==============================================================================
FileBuffer& FileBuffer::operator=(FileBuffer&& rhs)
{
  FileBuffer tmp(std::move(rhs));
  std::swap(m_handle, tmp.m_handle);
  std::swap(m_data, tmp.m_data);
  m_size = tmp.m_size;
  return *this;
}

} // XR
