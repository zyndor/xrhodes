#ifndef XR_COLLAGE_HPP
#define XR_COLLAGE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Image.hpp"
#include "xr/types/fundamentals.hpp"
#include <cstdint>

namespace xr
{
class AABB;

//=============================================================================
///@brief Manages an Image, allowing sequential addition of smaller sub-images
/// to it.
class Collage
{
  XR_NONCOPY_DECL(Collage)

public:
  // structors
  Collage();

  // general
  ///@brief Initializes the builder for an image of @a width x @a height pixels
  /// with the given @a stride.
  ///@param minBlockSize the minimum amount of pixels size of an area that allows
  /// the creation of a block.
  void Init(uint16_t width, uint16_t height, uint8_t stride, uint16_t minBlockSize = 2);  // TODO: padding

  ///@brief Attempts to allocate @a width x @a height (x stride) pixels of space
  /// on the atlas, and copy @a data from the sub-image of a @a pitch pixels wide
  /// larger image into it. Only if the operation has succeeded, the UVs relating
  /// to the resulting sub-image are written to @a outUVs.
  ///@return The success of the operation.
  ///@note The stride of the source data must match that of the builder.
  bool  Paste(uint16_t width, uint16_t height, uint16_t pitch, uint8_t const* data,
    xr::AABB& outUVs);

  ///@brief Attempts to allocate @a width x @a height (x stride) pixels of space
  /// on the atlas, and copy the given image @a data into it. Only if the operation
  /// has succeeded, the UVs relating to the resulting sub-image are written to
  /// @a outUVs.
  ///@return The success of the operation.
  ///@note The stride of the source data must match that of the builder.
  ///@note The pitch of the source data must match the width.
  bool  Paste(uint16_t width, uint16_t height, uint8_t const* data, xr::AABB& outUVs);

  ///@brief Performs allocation of @a width x @a height (x stride) pixels of space
  /// on the atlas. Only if the operation has succeeded, the UVs relating to the
  /// resulting sub-image are written to @a outUVs.
  ///@return Pointer to the start of the block. Nullptr signifies failure.
  uint8_t* Allocate(uint16_t width, uint16_t height, xr::AABB& outUVs);

  ///@return The image
  xr::Image const& GetImage() const;

  ///@brief Resets allocations, while preserving the format, size and image
  /// data.
  void  Reset();

private:
  // types
  struct Block
  {
    uint16_t  mX;
    uint16_t  mY;
    uint16_t  mWidth; // pixels; multiply by stride for bytes
    uint16_t  mHeight;
  };

  // data
  uint8_t mStride = 0;  // bytes
  uint16_t mMinBlockSize = 0; // pixels

  xr::Image mData;
  std::vector<Block>  mBlocks;
};

}

#endif //XR_COLLAGE_HPP
