//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Image.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/FileWriter.hpp"
#include "xr/ScopeGuard.hpp"
#include "xr/debug.hpp"
#include "png.h"

namespace xr
{

//==============================================================================
Image::Image ()
: m_width(0),
  m_height(0),
  m_bytesPerPixel(0),
  m_bytes()
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
uint32_t  Image::GetWidth() const
{
  return m_width;
}

//==============================================================================
uint32_t  Image::GetHeight() const
{
  return m_height;
}

//==============================================================================
uint8_t Image::GetBytesPerPixel() const
{
  return m_bytesPerPixel;
}

//==============================================================================
size_t  Image::GetStride() const
{
  return m_width * m_bytesPerPixel;
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
void Image::SetSize(uint32_t width, uint32_t height, uint8_t bytesPerPixel)
{
  m_width = width;
  m_height = height;
  m_bytesPerPixel = bytesPerPixel;
  m_bytes.resize(width * height * bytesPerPixel);
}

//==============================================================================
void Image::SetPixelData(uint8_t const* data, uint32_t width, uint32_t height,
  uint8_t bytesPerPixel)
{
  SetSize(width, height, bytesPerPixel);
  std::memcpy(m_bytes.data(), data, m_bytes.size());
}

//==============================================================================
bool  Image::Load(FilePath const& path)
{
  FileBuffer image;
  bool success = image.Open(path, false);
  if (success)
  {
    Parse(image.GetData(), image.GetSize());
  }
  return success;
}

//==============================================================================
bool Image::Save(FilePath path, bool overwrite)
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
static size_t const kPngSigBytes = 8;

void PngReadFn(png_structp pngPtr, png_bytep data, png_size_t size)
{
  png_voidp a = png_get_io_ptr(pngPtr);
  uint8_t const*& buffer = *reinterpret_cast<uint8_t const**>(a);
  memcpy(data, buffer, size);
  buffer += size;
}

bool Image::ParsePng(uint8_t const* buffer, size_t size)
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

  buffer += kPngSigBytes;
  png_set_read_fn(pngPtr, &buffer, PngReadFn);
  png_set_sig_bytes(pngPtr, kPngSigBytes);
  png_read_info(pngPtr, infoPtr);

  png_uint_32 pngWidth = png_get_image_width(pngPtr, infoPtr);
  png_uint_32 pngHeight = png_get_image_height(pngPtr, infoPtr);
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

  m_width = pngWidth;
  m_height = pngHeight;
  m_bytesPerPixel = pngChannels * pngBitsPerChannel / 8;

  const size_t  stride(pngWidth * m_bytesPerPixel);
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

bool Image::ParseTga(uint8_t const * buffer, size_t size)
{
  static_assert(sizeof(TgaHeader) == 18, "Update struct packing / alignment configuration or make sure that TgaHeader is read corretly.");

  TgaHeader const* header = reinterpret_cast<TgaHeader const*>(buffer);
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
  const uint16_t imageWidth = header->m_width[0] | (header->m_width[1] << 8);
  const uint16_t imageHeight = header->m_height[0] | (header->m_height[1] << 8);
  const uint8_t imageBpp = header->pixelBitDepth / 8;

  size_t stride = imageWidth * imageBpp;
  size_t imageDataSize = imageHeight * stride;

  buffer += header->idLength + sizeof(TgaHeader);
  m_bytes.resize(imageDataSize);

  // flip writer based on origin
  const uint16_t yOrigin = header->yOrigin[0] | (header->yOrigin[1] << 8);
  ptrdiff_t step = stride;
  buffer += yOrigin * stride;
  if(yOrigin > 0)
  {
    step = -step;
    buffer -= stride;
  }

  // copy rows to our buffer
  uint8_t* write = m_bytes.data();
  for(uint16_t i = 0; i < imageHeight; ++i)
  {
    memcpy(write, buffer, stride);
    write += stride;
    buffer += step;
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
static void PngWriteFn(png_structp pngPtr, png_bytep data, png_size_t size)
{
  png_voidp  a(png_get_io_ptr(pngPtr));
  static_cast<FileWriter*>(a)->Write(data, size, 1);
}

static void PngFlushFn(png_structp pngPtr)
{}

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

  png_set_write_fn(pngPtr, (void*)&w, PngWriteFn, PngFlushFn);

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
