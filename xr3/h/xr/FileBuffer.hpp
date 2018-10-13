#ifndef XR_FILEBUFFER_HPP
#define XR_FILEBUFFER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "File.hpp"
#include "xr/types/fundamentals.hpp"
#include <stdint.h>

namespace xr
{

//==============================================================================
///@brief Facilitates the reading of the whole of a file.
class FileBuffer
{
	XR_NONCOPY_DECL(FileBuffer)

public:
  // structors
  FileBuffer() = default;
  FileBuffer(FileBuffer&& rhs);
  ~FileBuffer();

  // general
  ///@brief Opens the file and reads all of its contents to an internal buffer.
  /// Any previously opened handle is closed, any previously owned buffer is
  /// deleted.
  ///@return Whether the operation was successful.
  bool  Open(FilePath const& path);

  ///@return The size of the file in bytes.
  size_t GetSize() const;

  ///@return A pointer to the buffer.
  ///@note Does not transfer ownership.
  uint8_t const*  GetData() const;

  ///@return Convenience to reinterpret_cast<T const*>(GetData()).
  ///@note Does not return ownership.
  template <typename T>
  T const*  CastData() const;

  ///@brief Transfers ownership of the buffer.
  ///@note GetSize() beforehand; the size will be 0 afterwards.
  uint8_t*  DisownData();

  ///@brief Closes the File::Handle, if it was open.
  void  Close();

  ///@brief Deletes the internal buffer, if there was any.
  void  Destroy();

  // operators
  FileBuffer& operator=(FileBuffer&& rhs);

private:
  // data
  File::Handle  m_handle = nullptr;
  size_t        m_size = 0;
  uint8_t*      m_data = nullptr;
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
