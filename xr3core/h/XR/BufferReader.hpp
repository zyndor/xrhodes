#ifndef XR_BUFFERREADER_HPP
#define XR_BUFFERREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <cstdint>

namespace XR
{

//==============================================================================
///@brief Facilitates linear reading of objects and bytes from a buffer of a
/// known size, preventing overruns.
class BufferReader
{
public:
  // structors
  explicit BufferReader(uint8_t const* buffer, size_t size)
  : m_buffer(buffer),
    m_size(size)
  {}

  // general
  ///@brief Attempts to read the next sizeof(T) bytes into the provided object
  /// @a val.
  ///@return The success of the operation, i.e. were there enough bytes?
  template <typename T>
  bool  Read(T& val)
  {
    bool result = sizeof(T) <= m_size;
    if(result)
    {
      memcpy(reinterpret_cast<uint8_t*>(&val), m_buffer, sizeof(T));
      m_buffer += sizeof(T);
      m_size -= sizeof(T);
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
    if(size <= m_size)
    {
      p = m_buffer;
      m_buffer += size;
      m_size -= size;
    }
    return p;
  }

  ///@return Number of bytes still readable.
  size_t GetRemainingSize() const
  {
    return m_size;
  }

private:
  // data
  uint8_t const* m_buffer;
  size_t m_size;
};

} // XR

#endif //XR_BUFFERREADER_HPP