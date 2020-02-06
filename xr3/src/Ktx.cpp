#include "Ktx.hpp"
#include "xr/Gfx.hpp"
#include "xr/Image.hpp"
#include "xr/File.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/memory/memory.hpp"
#include <unordered_map>
#include <stdint.h>

namespace xr
{

struct KtxHeader
{
  char id[12];
  uint32_t endianness;
  uint32_t glType;
  uint32_t glTypeSize;
  uint32_t glFormat;
  uint32_t glInternalFormat;
  uint32_t glBaseInternalFormat;
  uint32_t widthPx;
  uint32_t heightPx;
  uint32_t depthPx;
  uint32_t arraySize;
  uint32_t faceCount;
  uint32_t mipLevelCount;
  uint32_t dictSizeBytes;
};

const std::unordered_map<uint32_t, Gfx::TextureFormat> kFormats {
  //{ 0x93b0, GL_COMPRESSED_RGBA_ASTC_4x4_KHR }
  //{ 0x881b, GL_RGB16F },
  //{ 0x8815, GL_RGB32F },
  //{ 0x8c3a, GL_R11F_G11F_B10F },
  { 0x8d7c, Gfx::TextureFormat::RGBA8 },
  { 0x8d7d, Gfx::TextureFormat::RGB8 },
};

bool Ktx::Parse(size_t bufferSize, uint8_t const* buffer)
{
  KtxHeader header;
  BufferReader reader(Buffer{ bufferSize, buffer });
  if (!reader.Read(header))
  {
    XR_TRACE(Ktx, ("Invalid header."));
    return false;
  }

  auto iFormat = kFormats.find(header.glInternalFormat);
  if (iFormat == kFormats.end())
  {
    XR_TRACE(Ktx, ("Unsupported format: %x", header.glInternalFormat));
    return false;
  }
  mFormat = iFormat->second;

  header.widthPx = std::max(header.widthPx, 1u);
  header.heightPx = std::max(header.heightPx, 1u);
  if (header.widthPx > std::numeric_limits<uint16_t>::max() ||
    header.heightPx > std::numeric_limits<uint16_t>::max())
  {
    XR_TRACE(Ktx, ("Texture size excessive: %u x %u; 16bits maximum.",
      header.widthPx, header.heightPx));
    return false;
  }

  mWidthPx = header.widthPx;
  mHeightPx = header.heightPx;

  header.mipLevelCount = std::max(header.mipLevelCount, 1u);
  header.arraySize = std::max(header.arraySize, 1u);

  mFaceCount = header.faceCount;
  mMipLevelCount = header.mipLevelCount;

  // Calculate image buffer size.
  auto imageBufferSize = reader.GetRemainingSize();
  if (imageBufferSize < header.dictSizeBytes)
  {
    XR_TRACE(Ktx, ("Image buffer underflow by %d bytes.", header.dictSizeBytes -
      imageBufferSize));
    return false;
  }

  imageBufferSize -= header.dictSizeBytes;

  // Ignore dictionary.
  reader.ReadBytes(header.dictSizeBytes);

  // Copy image buffer
  auto imageBuffer = reader.ReadBytes(imageBufferSize);
  XR_ASSERT(Ktx, imageBuffer);  // only fail on underflow, which we've already checked.

  mData.resize(imageBufferSize);
  memcpy(mData.data(), imageBuffer, imageBufferSize);

  // Gfx expects image buffers per layer, then, within layer, per face.
  mImages.reserve(header.faceCount * header.mipLevelCount);

  // Reset the reader to the start of the image buffers, and process them.
  reader = BufferReader({ imageBufferSize, imageBuffer });
  for (uint32_t iMip = 0; iMip < header.mipLevelCount; ++iMip)
  {
    uint32_t imageSize;
    if (!reader.Read<uint32_t>(imageSize))
    {
      return false;
    }

    imageSize = Align(imageSize, 4u);

    // N.B. not sure what to do with additional elements; we've clamped this to a minimum of 1; more than 1 will be ignored [by Texture].
    for (uint32_t iArray = 0; iArray < header.arraySize; ++iArray)
    {
      for (uint32_t iFace = 0; iFace < header.faceCount; ++iFace)
      {
        auto image = reader.ReadBytes(imageSize);
        if (!image)
        {
          return false;
        }
        mImages.push_back({ imageSize, image });
      }
    }

    header.widthPx /= 2;
    header.heightPx /= 2;
  }

  return true;
}

}
