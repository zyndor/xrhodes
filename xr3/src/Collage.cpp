//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Collage.hpp"
#include "xr/AABB.hpp"
#include "xr/debug.hpp"
#include <cstring>

namespace xr
{

//=============================================================================
Collage::Collage()
{}

//=============================================================================
void Collage::Init(uint16_t width, uint16_t height, uint8_t stride, uint16_t minBlockSize)
{
  mStride = stride;
  mMinBlockSize = minBlockSize;

  mData.SetSize(width, height, stride);

  mBlocks.reserve(16);
  Reset();
}

//=============================================================================
bool Collage::Paste(uint16_t width, uint16_t height, uint16_t pitch,
  uint8_t const* data, xr::AABB& outUVs)
{
  auto writep = Allocate(width, height, outUVs);
  if (!writep)
  {
    return false;
  }

  // copy contents
  if (data)
  {
    auto atlasWidth = mData.GetWidth();
    const uint32_t readRowSize = pitch * mStride;
    const uint32_t writeRowSize = atlasWidth * mStride;
    for (uint16_t y = 0; y < height; ++y)
    {
      memcpy(writep, data, readRowSize);
      writep += writeRowSize;
      data += readRowSize;
    }
  }

  return true;
}

//=============================================================================
bool Collage::Paste(uint16_t width, uint16_t height, uint8_t const* data, xr::AABB& outUVs)
{
  return Paste(width, height, width, data, outUVs);
}

//=============================================================================
uint8_t* Collage::Allocate(uint16_t width, uint16_t height, xr::AABB& outUVs)
{
  XR_ASSERT(Collage, mData.GetPixelData());
  XR_ASSERT(Collage, mStride > 0);

  // find block with closest matching dimensions
  if (mBlocks.empty())
  {
    return nullptr;
  }

  Block* block = [this, width, height]() {
    std::vector<Block*> candidates;
    for (auto i = mBlocks.begin(), iEnd = mBlocks.end(); i != iEnd; ++i)
    {
      if (width <= i->mWidth && height <= i->mHeight) // if it fits
      {
        auto iInsert = std::upper_bound(candidates.begin(), candidates.end(), &*i,
          [width, height](Block const* bNew, Block const* bOld) {
          int32_t wDiffOld = int32_t(bOld->mWidth) - width;
          int32_t hDiffOld = int32_t(bOld->mHeight) - height;
          int32_t wDiffNew = int32_t(bNew->mWidth) - width;
          int32_t hDiffNew = int32_t(bNew->mHeight) - height;
          return wDiffOld + hDiffOld < wDiffNew + hDiffNew;
        });
        candidates.insert(iInsert, &*i);  // I sits
      }
    }
    return candidates.empty() ? nullptr : candidates.front();
  }();

  if (!block)
  {
    return nullptr;
  }

  XR_TRACE(Collage, ("Found candidate: x: %d, y: %d, width %d, height %d",
    block->mX, block->mY, block->mWidth, block->mHeight));

  // reshape blocks. trim everything to the left and above.
  for (auto& b : mBlocks)
  {
    if (b.mX < block->mX && b.mX + b.mWidth > b.mX && b.mY + b.mHeight <= block->mY)
    {
      XR_TRACE(Collage, ("Trimming block at %d, %d: %d, %d -> %d, %d",
        b.mX, b.mY, b.mWidth, b.mHeight, block->mX - b.mX, b.mHeight));
      b.mWidth = block->mX - b.mX;
    }

    if (b.mY < block->mY && b.mY + b.mHeight > b.mY && b.mX + b.mWidth <= block->mX)
    {
      XR_TRACE(Collage, ("Trimming block at %d, %d: %d, %d -> %d, %d",
        b.mX, b.mY, b.mWidth, b.mHeight, b.mWidth, block->mY - b.mY));
      b.mHeight = block->mY - b.mY;
    }
  }

  // create new blocks to the left and right, if remaining size is above threshold.
  auto atlasWidth = mData.GetWidth();
  uint16_t x = block->mX + width;
  if (atlasWidth - x > mMinBlockSize)
  {
    XR_TRACE(Collage, ("Creating block at %d, %d: %d, %d",
      x, block->mY, atlasWidth - x, block->mHeight));
    mBlocks.push_back(Block{ x, block->mY,
      static_cast<uint16_t>(atlasWidth - x), block->mHeight });
  }

  auto atlasHeight = mData.GetHeight();
  uint16_t y = block->mY + height;
  if (atlasHeight - y > mMinBlockSize)
  {
    XR_TRACE(Collage, ("Creating block at %d, %d: %d, %d",
      block->mX, y, block->mWidth, atlasHeight - y));
    mBlocks.push_back(Block{ block->mX, y,
      block->mWidth, static_cast<uint16_t>(atlasHeight - y) });
  }

  auto blockSave = *block;
  mBlocks.erase(std::find_if(mBlocks.begin(), mBlocks.end(), [block](Block const& b) {
    return &b == block;
  }));

  // sort blocks
  std::sort(mBlocks.begin(), mBlocks.end(), [](Block const& b0, Block const& b1) {
    return b0.mWidth * b0.mHeight < b1.mWidth * b1.mHeight;
  });

  auto iRemove = std::remove_if(mBlocks.begin(), mBlocks.end(),
    [this](Block const& block) {
    auto index = &block - mBlocks.data();
    auto iter = mBlocks.begin();
    std::advance(iter, index + 1);
    return std::find_if(iter, mBlocks.end(), [&block](Block const& other) {
      return block.mX >= other.mX && block.mY >= other.mY &&
        block.mWidth <= other.mWidth && block.mHeight <= other.mHeight;
    }) != mBlocks.end();
  });
  mBlocks.erase(iRemove, mBlocks.end());

  // determine uv results
  outUVs.left = blockSave.mX / float(atlasWidth);
  outUVs.bottom = blockSave.mY / float(atlasHeight);
  outUVs.right = (blockSave.mX + width) / float(atlasWidth);
  outUVs.top = (blockSave.mY + height) / float(atlasHeight);

  uint8_t* writep = mData.GetPixelData() + (blockSave.mY * atlasWidth + blockSave.mX) * mStride;
  return writep;
}

//=============================================================================
xr::Image const& Collage::GetImage() const
{
  return mData;
}

//=============================================================================
void Collage::Reset()
{
  mBlocks.clear();
  mBlocks.push_back({ 0, 0, static_cast<uint16_t>(mData.GetWidth()),
    static_cast<uint16_t>(mData.GetHeight()) });
}

}
