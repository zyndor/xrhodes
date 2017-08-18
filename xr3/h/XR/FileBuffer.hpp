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
#include <stdint.h>

namespace XR
{

//==============================================================================
///@brief Facilitates the reading of the whole of a file.
class FileBuffer
{
	XR_NONCOPY_DECL(FileBuffer)

public:
  // structors
  FileBuffer();
  ~FileBuffer();

  // general
  bool            Open(File::Path const& path, bool text);

  size_t          GetSize() const;
  uint8_t const*  GetData() const;

  template <typename T>
  T const*        CastData() const;

  ///@brief Transfers ownership of the buffer.
  ///@note GetSize() beforehand; the size will be 0 afterwards.
  uint8_t*        DisownData();

  void            Close();
  void            Destroy();

private:
  // data
  File::Handle  m_handle;
  size_t        m_size;
  uint8_t*      m_data;
};

//==============================================================================
// implementation
//==============================================================================
inline
size_t FileBuffer::GetSize() const
{
  return m_size;
}

//==============================================================================
inline
const uint8_t* FileBuffer::GetData() const
{
  return m_data;
}

//==============================================================================
template <typename T>
inline
T const* FileBuffer::CastData() const
{
  return reinterpret_cast<T const*>(m_data);
}

} // XR

#endif  //XR_FILEBUFFER_HPP
