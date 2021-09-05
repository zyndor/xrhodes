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
    auto stride = mData.GetBytesPerPixel();
    const uint32_t readRowSize = pitch * stride;
    const uint32_t writeRowSize = atlasWidth * stride;
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
  auto stride = mData.GetBytesPerPixel();
  XR_ASSERT(Collage, stride > 0);

  // find block with closest matching dimensions
  if (mBlocks.empty() || width * height == 0)
  {
    return nullptr;
  }

  // attempting to create a width x height pixels block.
  Block* block = [this, width, height]() {
    // TODO: use a binary heap instead
    std::vector<Block*> candidates;
    for (auto& i: mBlocks)
    {
      if (width <= i.width && height <= i.height) // if it fits
      {
        // order candidates by which one is closer in size to requested block.
        // TODO: those that are too small (negative diff) should be discarded).
        auto iInsert = std::upper_bound(candidates.begin(), candidates.end(), &i,
          [width, height](Block const* bNew, Block const* bOld) {
            int32_t wDiffOld = int32_t(bOld->width) - width;
            int32_t hDiffOld = int32_t(bOld->height) - height;
            int32_t wDiffNew = int32_t(bNew->width) - width;
            int32_t hDiffNew = int32_t(bNew->height) - height;
            return wDiffOld + hDiffOld < wDiffNew + hDiffNew;
          });
        candidates.insert(iInsert, &i);  // I sits
      }
    }
    return candidates.empty() ? nullptr : candidates.front();
  }();

  if (!block)
  {
    return nullptr;
  }

  XR_TRACE(Collage, ("Found candidate: x: %d, y: %d, width %d, height %d",
    block->x, block->y, block->width, block->height));

  // reshape blocks. trim everything to the left and above.
  for (auto& b : mBlocks)
  {
    if (b.x < block->x && b.x + b.width > block->x && b.y + b.height <= block->y)
    {
      XR_TRACE(Collage, ("Trimming block at %d, %d: %d, %d -> %d, %d",
        b.x, b.y, b.width, b.height, block->x - b.x, b.height));
      b.width = block->x - b.x;
    }

    if (b.y < block->y && b.y + b.height > block->y && b.x + b.width <= block->x)
    {
      XR_TRACE(Collage, ("Trimming block at %d, %d: %d, %d -> %d, %d",
        b.x, b.y, b.width, b.height, b.width, block->y - b.y));
      b.height = block->y - b.y;
    }
  }

  // create new blocks to the left and right, if remaining size is above threshold.
  auto atlasWidth = mData.GetWidth();
  uint16_t x = block->x + width;
  if (atlasWidth - x > mMinBlockSize)
  {
    XR_TRACE(Collage, ("Creating block at %d, %d: %d, %d",
      x, block->y, atlasWidth - x, block->height));
    mBlocks.push_back(Block{ x, block->y,
      static_cast<uint16_t>(atlasWidth - x), block->height });
  }

  auto atlasHeight = mData.GetHeight();
  uint16_t y = block->y + height;
  if (atlasHeight - y > mMinBlockSize)
  {
    XR_TRACE(Collage, ("Creating block at %d, %d: %d, %d",
      block->x, y, block->width, atlasHeight - y));
    mBlocks.push_back(Block{ block->x, y,
      block->width, static_cast<uint16_t>(atlasHeight - y) });
  }

  auto blockSave = *block;
  mBlocks.erase(std::find_if(mBlocks.begin(), mBlocks.end(), [block](Block const& b) {
    return &b == block;
  }));

  // sort blocks
  std::sort(mBlocks.begin(), mBlocks.end(), [](Block const& b0, Block const& b1) {
    return b0.width * b0.height < b1.width * b1.height;
  });

  auto iRemove = std::remove_if(mBlocks.begin(), mBlocks.end(),
    [this](Block const& b) {
      auto index = &b - mBlocks.data();
      auto iter = mBlocks.begin();
      std::advance(iter, index + 1);
      return std::find_if(iter, mBlocks.end(), [&b](Block const& other) {
        return b.x >= other.x && b.y >= other.y &&
          b.width <= other.width && b.height <= other.height;
      }) != mBlocks.end();
    });
  mBlocks.erase(iRemove, mBlocks.end());

  // determine uv results
  outUVs.left = blockSave.x / float(atlasWidth);
  outUVs.bottom = blockSave.y / float(atlasHeight);
  outUVs.right = (blockSave.x + width) / float(atlasWidth);
  outUVs.top = (blockSave.y + height) / float(atlasHeight);

  uint8_t* writep = mData.GetPixelData() + (blockSave.y * atlasWidth + blockSave.x) * stride;
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
