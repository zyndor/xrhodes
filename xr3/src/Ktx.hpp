#ifndef XR_KTX_HPP
#define XR_KTX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Gfx.hpp"
#include <vector>

namespace xr
{
struct Buffer;
class FilePath;
class Image;

struct Ktx
{
  Gfx::TextureFormat  mFormat;
  uint16_t mFaceCount;
  uint16_t mMipLevelCount;
  uint16_t mWidthPx;  // of top level
  uint16_t mHeightPx; // of top level
  // N.B. ignoring depth
  std::vector<uint8_t>  mData;
  std::vector<Buffer> mImages;  // references mData, size is mFaceCount * mMipLevelCount, index as mImage[mipLevel * mMipLevelCount + face];

  bool  Parse(size_t bufferSize, uint8_t const* buffer);
};

}

#endif //XR_KTX_HPP
