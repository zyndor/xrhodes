//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Image.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/FileWriter.hpp"
#include "xr/memory/Buffer.hpp"
#include "xr/memory/ScopeGuard.hpp"
#include "xr/debug.hpp"
#include "xr/types/intutils.hpp"
#include "png.h"
#include <cstring>

// setjmp - a libPNG requirement - is causing trouble when extra warnings are enabled,
// so we're going to have to silence them here. Note that we're on the good side
// of the rules, as we're not creating C++ objects (with non-trivial destructors)
// between setjmp() and longjmp(), nor are we trying to use any of the variables
// in the scope after the longjmp().
#if defined XR_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4611)  // setjmp - C++ object interaction not portable.
#elif defined XR_COMPILER_GCC
#pragma GCC diagnostic ignored "-Wclobbered"
#endif

namespace xr
{

static_assert(std::is_pod<Buffer>::value);

//==============================================================================
Image::Image ()
: m_width(0),
  m_height(0),
  m_bytesPerPixel(0),
  m_bytes()
{}

//==============================================================================
Image::Image(Image&& other)
: m_width(other.m_width),
  m_height(other.m_height),
  m_bytesPerPixel(std::move(other.m_bytesPerPixel)),
  m_bytes(std::move(other.m_bytes))
{}

//==============================================================================
Image::Image(const Image& rhs)
: m_width(rhs.m_width),
  m_height(rhs.m_height),
  m_bytesPerPixel(rhs.m_bytesPerPixel),
  m_bytes(rhs.m_bytes)
{}

//==============================================================================
Image::~Image ()
{}

//==============================================================================
uint16_t  Image::GetWidth() const
{
  return m_width;
}

//==============================================================================
uint16_t  Image::GetHeight() const
{
  return m_height;
}

//==============================================================================
uint8_t Image::GetBytesPerPixel() const
{
  return m_bytesPerPixel;
}

//==============================================================================
size_t Image::GetPitch() const
{
  return m_width * m_bytesPerPixel;
}

//==============================================================================
size_t Image::GetStride() const
{
  return GetPitch();
}

//==============================================================================
uint8_t const* Image::GetPixelData() const
{
  return m_bytes.data();
}

//==============================================================================
uint8_t* Image::GetPixelData()
{
  return m_bytes.data();
}

//==============================================================================
size_t Image::GetPixelDataSize() const
{
  return m_bytes.size();
}

//==============================================================================
void Image::SetSize(uint16_t width, uint16_t height, uint8_t bytesPerPixel)
{
  m_width = width;
  m_height = height;
  m_bytesPerPixel = bytesPerPixel;
  m_bytes.resize(width * height * bytesPerPixel);
}

//==============================================================================
void Image::SetPixelData(uint8_t const* data, uint16_t width, uint16_t height,
  uint8_t bytesPerPixel)
{
  SetSize(width, height, bytesPerPixel);
  std::memcpy(m_bytes.data(), data, m_bytes.size());
}

//==============================================================================
bool  Image::Load(FilePath const& path)
{
  FileBuffer image;
  bool success = image.Open(path);
  if (success)
  {
    Parse(image.GetData(), image.GetSize());
  }
  return success;
}

//==============================================================================
bool Image::Save(FilePath path, bool overwrite) const
{
  auto ext = path.GetExt();
  bool success = ext != nullptr;
  if (success)
  {
    bool(Image::*fn)(FileWriter&) const = nullptr;
    if (strcmp(ext, "png") == 0)
    {
      fn = &Image::WritePng;
    }
    else if (strcmp(ext, "tga") == 0)
    {
      fn = &Image::WriteTga;
    }

    FileWriter writer;
    success = fn != nullptr &&
      (!File::CheckExists(path) || overwrite) &&
      writer.Open(path, FileWriter::Mode::Truncate, false) &&
      (this->*fn)(writer);
  }
  return success;
}

//==============================================================================
bool Image::Parse(uint8_t const* buffer, size_t size)
{
  Reset();
  bool result = true;
  if(png_sig_cmp(buffer, 0, size) == 0)
  {
    result = ParsePng(buffer, size);
  }
  // else if other format, probably parse before tga, since we can't really check for that.
  else
  {
    result = ParseTga(buffer, size);
  }

  return result;
}

//==============================================================================
void  Image::Copy(const Image& img)
{
  Image tmp(img);

  Swap(tmp);
}

//==============================================================================
void  Image::Swap(Image &rhs)
{
  std::swap(m_width, rhs.m_width);
  std::swap(m_height, rhs.m_height);
  std::swap(m_bytesPerPixel, rhs.m_bytesPerPixel);
  m_bytes.swap(rhs.m_bytes);
}

//==============================================================================
void Image::Reset()
{
	m_width = 0;
	m_height = 0;
	m_bytesPerPixel = 0;
	m_bytes.clear();
}

//==============================================================================
Image& Image::operator=(Image const& other)
{
  Image tmp(other);
  Swap(tmp);
  return *this;
}

//==============================================================================
Image& Image::operator=(Image&& other)
{
  Image tmp(other);
  Swap(tmp);
  return *this;
}

//==============================================================================
namespace
{

size_t const kPngSigBytes = 8;

void PngReadFn(png_structp pngPtr, png_bytep data, png_size_t size)
{
  png_voidp userData{ png_get_io_ptr(pngPtr) };
  Buffer& buffer = *reinterpret_cast<Buffer*>(userData);
  if (buffer.size >= size)
  {
    memcpy(data, buffer.data, size);
    buffer.data += size;
    buffer.size -= size;
  }
  else
  {
    png_chunk_error(pngPtr, "Unexpected end of buffer.");
  }
}

}

bool Image::ParsePng(uint8_t const* data, size_t size)
{
  png_structp  pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
    nullptr, nullptr);
  if (!pngPtr)
  {
    XR_TRACE(Image, ("Failed to initialize PNG read struct."));
    return false;
  }

  png_infop infoPtr = png_create_info_struct(pngPtr);
  if (!infoPtr)
  {
    XR_TRACE(Image, ("Failed to initialize PNG info struct."));
    return false;
  }

  auto readStructGuard = MakeScopeGuard([&pngPtr, &infoPtr] {
    png_destroy_read_struct(&pngPtr, &infoPtr, png_infopp(0));
  });

  std::vector<png_bytep> rowPtrs;
  if (setjmp(png_jmpbuf(pngPtr)))
  {
    XR_TRACE(Image, ("An error occured while reading the PNG file."));
    return false;
  }

  data += kPngSigBytes;
  size -= kPngSigBytes;
  Buffer buffer{ size, data };
  png_set_read_fn(pngPtr, &buffer, PngReadFn);
  png_set_sig_bytes(pngPtr, kPngSigBytes);
  png_read_info(pngPtr, infoPtr);

  png_uint_32 pngWidth = png_get_image_width(pngPtr, infoPtr);
  png_uint_32 pngHeight = png_get_image_height(pngPtr, infoPtr);
  if (!(Representable<Px>(pngWidth) && Representable<Px>(pngHeight)))
  {
    XR_TRACE(Image, ("Image size %u x %u excessive; 16 bits max.", pngWidth, pngHeight));
    return false;
  }

  png_uint_32 pngBitsPerChannel = png_get_bit_depth(pngPtr, infoPtr);
  png_uint_32 pngChannels = png_get_channels(pngPtr, infoPtr);
  png_byte  colorType = png_get_color_type(pngPtr, infoPtr);

  switch (colorType)
  {
  case  PNG_COLOR_TYPE_PALETTE:
    png_set_palette_to_rgb(pngPtr);
    pngChannels = 3;
    pngBitsPerChannel = 8;
    break;

  case  PNG_COLOR_TYPE_GRAY:
    if (pngBitsPerChannel < 8)
    {
      png_set_gray_to_rgb(pngPtr);
      pngBitsPerChannel = 8;
    }
    break;
  }

  if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
  {
    png_set_tRNS_to_alpha(pngPtr);
    ++pngChannels;
  }

  if (pngBitsPerChannel == 16)
  {
    png_set_strip_16(pngPtr);
  }

  rowPtrs.resize(pngHeight);

  m_width = Px(pngWidth);
  m_height = Px(pngHeight);
  const uint32_t bytesPerPixel = pngChannels * pngBitsPerChannel / 8;
  if (bytesPerPixel > 0xff)
  {
    XR_TRACE(Image, ("Invalid bytes-per-pixel value: %d.", bytesPerPixel));
    return false;
  }

  m_bytesPerPixel = uint8_t(bytesPerPixel);

  const size_t  stride(pngWidth * bytesPerPixel);
  m_bytes.resize(pngHeight * stride);

  for (size_t i = 0; i < pngHeight; ++i)
  {
    size_t  q = (pngHeight - i - 1) * stride;
    rowPtrs[i] = static_cast<png_bytep>(m_bytes.data()) + q;
  }

  png_read_image(pngPtr, rowPtrs.data());

  return true;
}

//==============================================================================
namespace
{

struct TgaHeader
{
  uint8_t idLength;
  uint8_t colorMapType;
  uint8_t imageType;
  uint8_t colorMapSpecs[5];
  uint8_t xOrigin[2];
  uint8_t yOrigin[2];
  uint8_t m_width[2];
  uint8_t m_height[2];
  uint8_t pixelBitDepth;
  uint8_t descriptorByte;
};

uint16_t ReadBigEndianShort(uint8_t const (&bytes)[2])
{
  return bytes[0] | (bytes[1] << 8);
}

}

bool Image::ParseTga(uint8_t const* data, size_t size)
{
  static_assert(sizeof(TgaHeader) == 18, "Update struct packing / alignment configuration or make sure that TgaHeader is read corretly.");

  TgaHeader const* header = reinterpret_cast<TgaHeader const*>(data);
  if (header->colorMapType != 0)
  {
    XR_TRACE(Image, ("Colourmapped TGA image -- not supported."));
    return false;
  }

  if (!(header->imageType == 2 || header->imageType == 3))
  {
    XR_TRACE(Image, ("Compressed TGA image (type %d) -- not supported.", header->imageType));
    return false;
  }

  if ((header->pixelBitDepth & 0x3) != 0)
  {
    XR_TRACE(Image, ("Unsupported TGA bit depth (%d).", header->pixelBitDepth));
    return false;
  }

  // m_width and m_height are 2 m_bytes
  const uint16_t imageWidth = ReadBigEndianShort(header->m_width);
  const uint16_t imageHeight = ReadBigEndianShort(header->m_height);
  const uint8_t imageBpp = header->pixelBitDepth / 8;

  size_t stride = imageWidth * imageBpp;
  size_t imageDataSize = imageHeight * stride;

  const auto skipHeaderAndId = header->idLength + sizeof(TgaHeader);
  if (skipHeaderAndId > size)
  {
    XR_TRACE(Image, ("Unexpected end of TGA buffer."));
    return false;
  }

  data += skipHeaderAndId;
  size -= skipHeaderAndId;
  m_bytes.resize(imageDataSize);

  // flip writer based on origin
  const uint16_t yOrigin = ReadBigEndianShort(header->yOrigin);
  ptrdiff_t step = stride;
  data += yOrigin * stride;
  if(yOrigin > 0)
  {
    step = -step;
    data -= stride;
  }

  // copy rows to our buffer
  uint8_t* write = m_bytes.data();
  for(uint16_t i = 0; i < imageHeight; ++i)
  {
    memcpy(write, data, stride);
    write += stride;
    data += step;
  }

  // convert from BGR(A) to RGB(A).
  if (imageBpp > 2)
  {
    for (auto i0 = m_bytes.data(), i1 = m_bytes.data() + m_bytes.size();
      i0 != i1; i0 += imageBpp)
    {
      std::swap(*i0, *(i0 + 2));
    }
  }

  m_width = imageWidth;
  m_height = imageHeight;
  m_bytesPerPixel = imageBpp;

  return true;
}

//==============================================================================
namespace
{

void PngWriteFn(png_structp pngPtr, png_bytep data, png_size_t size)
{
  png_voidp userData{ png_get_io_ptr(pngPtr) };
  static_cast<FileWriter*>(userData)->Write(data, size, 1);
}

void PngFlushFn(png_structp /*pngPtr*/)
{}

}

bool Image::WritePng(FileWriter& w) const
{
  png_structp  pngPtr(png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
    NULL));
  if (!pngPtr)
  {
    XR_TRACE(Image, ("Failed to initialize PNG write struct."));
    return false;
  }

  png_infop  infoPtr(png_create_info_struct(pngPtr));
  if (!infoPtr)
  {
    XR_TRACE(Image, ("Failed to initialize PNG info struct."));
    return false;
  }

  auto writeStructGuard = MakeScopeGuard([&pngPtr, &infoPtr] {
    png_destroy_write_struct(&pngPtr, &infoPtr);
  });

  std::vector<png_bytep> rowPtrs;
  if (setjmp(png_jmpbuf(pngPtr)))
  {
    XR_TRACE(Image, ("An error occured while writing the PNG file."));
    return false;
  }

  png_set_write_fn(pngPtr, &w, PngWriteFn, PngFlushFn);

  int  colorType(0);
  switch (m_bytesPerPixel)
  {
  case 4:
    colorType = PNG_COLOR_TYPE_RGBA;
    break;

  case 3:
    colorType = PNG_COLOR_TYPE_RGB;
    break;

  case 1:
    colorType = PNG_COLOR_TYPE_GRAY;
    break;
  }

  png_set_IHDR(pngPtr, infoPtr, png_uint_32(m_width), png_uint_32(m_height), 8,
    colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT);

  png_write_info(pngPtr, infoPtr);

  rowPtrs.resize(m_height);
  const uint32_t stride = m_width * m_bytesPerPixel;
  auto pixelData = const_cast<uint8_t*>(m_bytes.data());  // Sorry, old chap.
  for (uint32_t i = 0; i < m_height; ++i)
  {
    uint32_t q = (m_height - i - 1) * stride;
    rowPtrs[i] = static_cast<png_bytep>(pixelData) + q;
  }

  png_write_image(pngPtr, rowPtrs.data());
  png_write_end(pngPtr, NULL);

  return true;
}

//==============================================================================
bool Image::WriteTga(FileWriter& w) const
{
  TgaHeader header = {
    0,  // image id field length
    0,  // not colormapped
    static_cast<uint8_t>(m_bytesPerPixel > 1 ? 2 : 3),
    { 0 },  // no colormap
    { 0 },  // top...
    { 0 },  // ...left based
    { static_cast<uint8_t>(m_width & 0xff),
      static_cast<uint8_t>((m_width >> 8) & 0xff) },
    { static_cast<uint8_t>(m_height & 0xff),
      static_cast<uint8_t>((m_height >> 8) & 0xff) },
    static_cast<uint8_t>(m_bytesPerPixel * 8),
    0
  };

  bool success = w.Write(header);
  if (m_bytesPerPixel == 1)
  {
    success = w.Write(m_bytes.data(), 1, m_bytes.size());
  }
  else
  {
    uint8_t swapBuffer[4];
    if (m_bytesPerPixel == 3) // Write BGR
    {
      for (auto i0 = m_bytes.data(), i1 = m_bytes.data() + m_bytes.size();
        success && i0 != i1; i0 += 3)
      {
        swapBuffer[0] = i0[2];
        swapBuffer[1] = i0[1];
        swapBuffer[2] = i0[0];
        success = w.Write(swapBuffer, 3, 1);
      }
    }
    else if (m_bytesPerPixel == 4)  // write BGRA
    {
      for (auto i0 = m_bytes.data(), i1 = m_bytes.data() + m_bytes.size();
        success && i0 != i1; i0 += 4)
      {
        swapBuffer[0] = i0[2];
        swapBuffer[1] = i0[1];
        swapBuffer[2] = i0[0];
        swapBuffer[3] = i0[3];
        success = w.Write(swapBuffer, 4, 1);
      }
    }
    else
    {
      success = false;
    }
  }

  return success;
}

} // XR
