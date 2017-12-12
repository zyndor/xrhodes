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
  size_t size;
  uint8_t const* data;
};

} // XR

#endif //XR_BUFFERREADER_HPP