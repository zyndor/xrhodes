#ifndef XR_IMAGE_HPP
#define XR_IMAGE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Px.hpp"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace xr
{

class FilePath;
class FileWriter;

//==============================================================================
///@brief Pixel data and dimensions of an image with capabilities to load it
/// from a PNG or TGA file.
class Image
{
public:
  // structors
  Image();
  Image(Image&& other);
  Image(Image const& other);
  ~Image();

  // general
  ///@return  The width of the image in pixels.
  Px GetWidth() const;

  ///@return  The height of the image in pixels.
  Px GetHeight() const;

  ///@return  Number of bytes per pixel.
  uint8_t GetBytesPerPixel() const;

  ///@return Length of a single row in the pixel data, in bytes.
  size_t GetPitch() const;

  [[deprecated("Use GetPitch().")]]
  size_t GetStride() const;

  ///@return  Pointer to native pixel data.
  uint8_t const* GetPixelData() const;

  ///@return  Pointer to native pixel data.
  uint8_t* GetPixelData();

  ///@return  Pixel data size in bytes.
  size_t GetPixelDataSize() const;

  ///@brief Sets the size and bit depth of the image, allocating a large
  /// enough buffer to store an image of the given parameters.
  void SetSize(Px width, Px height, uint8_t bytesPerPixel);

  ///@brief Sets pixel data directly.
  void SetPixelData(uint8_t const* data, Px width, Px height,
    uint8_t bytesPerPixel);

  ///@brief Loads image from file at @a path. Currently TGA and PNG
  /// formats are supported.
  ///@return The result of the operation.
  bool Load(FilePath const& path);

  ///@brief Writes image to a location @a path. TGA and PNG formats are
  /// supported; the one used will be picked based on the extension of the
  /// given @a path. If a file of the same name exists, the method will fail
  /// unless @a overwrite was set to true.
  ///@return The result of the operation.
  bool Save(FilePath path, bool overwrite) const;

  ///@brief Attemts to parse image information from a buffer.
  ///@note Currently PNG and TGA are the only supported formates.
  bool Parse(uint8_t const* data, size_t size);

  ///@brief Copies @a img, overwriting this image.
  void Copy(const Image& img);

  ///@brief Efficient swapping of image objects.
  void Swap(Image& rhs);

  ///@return Clears the pixel data and resets dimensions to 0.
  ///@note Doesn't deallocate the buffer of pixel data. Image().Swap(img) may be
  /// used to work around this.
  void Reset();

  Image& operator=(Image const& other);
  Image& operator=(Image&& other);

private:
  // data
  Px m_width;
  Px m_height;
  uint8_t m_bytesPerPixel;
  std::vector<uint8_t> m_bytes;

  // internal
  bool ParsePng(uint8_t const* data, size_t size);
  bool ParseTga(uint8_t const* data, size_t size);

  bool WritePng(FileWriter& w) const;
  bool WriteTga(FileWriter& w) const;
};

} // XR

#endif // XR_IMAGE_HPP
