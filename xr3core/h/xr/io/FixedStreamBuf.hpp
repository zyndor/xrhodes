#ifndef XR_FIXEDSTREAMBUF_HPP
#define XR_FIXEDSTREAMBUF_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <streambuf>

namespace xr
{

//==============================================================================
///@brief streambuf specialisation that uses the buffer provided, never making
/// an allocation. Stream insertions past the initial capacity will fail.
class FixedStreamBuf : public std::streambuf
{
public:
  FixedStreamBuf(size_t size, char* buffer)
  {
    setp(buffer, buffer + size);
  }
};

} // xr

#endif //XR_FIXEDSTERAMBUF
