#ifndef XR_BUFFER_HPP
#define XR_BUFFER_HPP
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
struct Buffer
{
  // static
  ///@brief Convenience method to create a buffer from an array.
  template <typename T, size_t n>
  inline
  static Buffer FromArray(T(&ar)[n])
  {
    return { sizeof(ar), reinterpret_cast<const uint8_t*>(ar) };
  }

  // data
  size_t size;
  uint8_t const* data;

  // general
  template <typename T>
  T* As()
  {
    return reinterpret_cast<T*>(data);
  }

  template <typename T>
  T const* As() const
  {
    return reinterpret_cast<T const*>(data);
  }
};

} // XR

#endif //XR_BUFFERREADER_HPP
