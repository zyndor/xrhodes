#ifndef XR_BUFFERREADER_HPP
#define XR_BUFFERREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Buffer.hpp"

namespace XR
{

//==============================================================================
///@brief Facilitates linear reading of objects and bytes from a buffer of a
/// known size, preventing overruns.
class BufferReader
{
public:
  // structors
  BufferReader(Buffer const& buffer)
  : m_buffer(buffer)
  {}

  // general
  ///@brief Attempts to read the next sizeof(T) bytes into the provided object
  /// @a val.
  ///@return The success of the operation, i.e. were there enough bytes?
  template <typename T>
  bool  Read(T& val)
  {
    bool result = sizeof(T) <= m_buffer.size;
    if (result)
    {
      memcpy(reinterpret_cast<uint8_t*>(&val), m_buffer.data, sizeof(T));
      m_buffer.data += sizeof(T);
      m_buffer.size -= sizeof(T);
    }
    return result;
  }

  ///@brief Attempts to read the next @a size bytes and returns the pointer to
  /// this sub-buffer.
  ///@return A const pointer to a buffer of @a size bytes if successful;
  /// nullptr if not (there were not enough bytes).
  uint8_t const* ReadBytes(size_t size)
  {
    uint8_t const* p = nullptr;
    if (size <= m_buffer.size)
    {
      p = m_buffer.data;
      m_buffer.data += size;
      m_buffer.size -= size;
    }
    return p;
  }

  ///@return Number of bytes still readable.
  size_t GetRemainingSize() const
  {
    return m_buffer.size;
  }

private:
  // data
  Buffer m_buffer;
};

}

#endif
