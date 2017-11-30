//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Texture.hpp"
#include "XR/Image.hpp"
#include "XR/ParserCore.hpp"
#include "XR/BufferReader.hpp"
#include "XR/FileWriter.hpp"

namespace XR
{

//==============================================================================
XR_ASSET_DEF(Texture, "xtex", 1, "png;tga"/*";ktx"*/)

//==============================================================================
namespace {

XR_ASSET_BUILDER_DECL(Texture)

#ifdef ENABLE_ASSET_BUILDING
XR_ASSET_BUILDER_BUILD_SIG(Texture)
{
  // parse image
  Image img;
  bool success = img.Parse(buffer, size);
  if (success)
  {
    uint32_t width = img.GetWidth();
    uint32_t height = img.GetHeight();
    uint8_t bytesPerPixel = img.GetBytesPerPixel();
    success = data.write(reinterpret_cast<char const*>(&width), sizeof(width)).
      write(reinterpret_cast<char const*>(&height), sizeof(height)).
      write(reinterpret_cast<char const*>(&bytesPerPixel), sizeof(bytesPerPixel)).
      write(reinterpret_cast<char const*>(img.GetPixelData()), img.GetPixelDataSize()).
      good();
  }
  return success;
}
#endif

}

//==============================================================================
void Texture::Bind(uint32_t stage) const
{
  Gfx::SetTexture(m_handle, stage);
}

//==============================================================================
bool Texture::OnLoaded(size_t size, uint8_t const* buffer)
{
  OnUnload(); // TODO - move to Asset::Load()?

  bool success = true;
  BufferReader reader(buffer, size);

  uint32_t width = 0;
  uint32_t height = 0;
  uint8_t bytesPerPixel = 0;
  if (success)  // read texture dimensions
  {
    success = reader.Read(width) && reader.Read(height) &&
      reader.Read(bytesPerPixel);
  }

  if (success) // read texture flags
  {
    // Translate Asset flags to Gfx texture flags.
    uint32_t flags = GetFlags();
    uint32_t textureFlags = 0;
    if (CheckAllMaskBits(flags, WrapFlag))
    {
      textureFlags |= Gfx::F_TEXTURE_WRAP;
    }
    if (CheckAllMaskBits(flags, PointFilterFlag))
    {
      textureFlags |= Gfx::F_TEXTURE_FILTER_POINT;
    }
    if (CheckAllMaskBits(flags, MipMapsFlag))
    {
      textureFlags |= Gfx::F_TEXTURE_MIPMAPS;
    }
    if (CheckAllMaskBits(flags, SrgbFlag))
    {
      textureFlags |= Gfx::F_TEXTURE_SRGB;
    }

    // TOOD: support compressed textures. encode in bytesPerPixel, e.g. 0x80?
    Gfx::TextureFormat  format;
    switch (bytesPerPixel)
    {
    case 1:
      format = Gfx::TextureFormat::R8;
      break;
    case 3:
      format = Gfx::TextureFormat::RGB8;
      break;
    case 4:
      format = Gfx::TextureFormat::RGBA8;
      break;
    }

    // Create Gfx texture.
    const size_t texDataSize = width * height * bytesPerPixel;
    Gfx::Buffer texBuffer = { reader.ReadBytes(texDataSize), texDataSize };
    success = texBuffer.data != nullptr;
    if (success)  // set properties
    {
      m_width = width;
      m_height = height;
      m_hasAlpha = bytesPerPixel == 4;
      m_handle = Gfx::CreateTexture(format, width, height, 0, textureFlags, &texBuffer);

      if (CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
      {
        m_data.assign(buffer, buffer + size);
      }
    }
  }
  return success;
}

//==============================================================================
void Texture::OnUnload()
{
  m_width = 0;
  m_height = 0;
  m_hasAlpha = false;

  Gfx::Destroy(m_handle);
  m_handle.Invalidate();

  std::vector<uint8_t>().swap(m_data);
}

} // XR