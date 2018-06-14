//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Texture.hpp"
#include "xr/Image.hpp"
#include "xr/ParserCore.hpp"
#include "xr/BufferReader.hpp"
#include "xr/FileWriter.hpp"
#include "xr/streamutils.hpp"

#define LTRACE(format) XR_TRACE(Texture, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Texture, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Texture, "xtex", 2, "png;tga"/*";ktx"*/)

//==============================================================================
namespace {

bool SerializeTexture(Gfx::TextureFormat format, uint32_t width, uint32_t height,
  Buffer const& pixelData, std::ostream& dataOut)
{
  return WriteBinaryStream(format, dataOut) &&
    WriteBinaryStream(width, dataOut) &&
    WriteBinaryStream(height, dataOut) &&
    WriteRangeBinaryStream<uint32_t>(pixelData.data, pixelData.data + pixelData.size, dataOut);
}

XR_ASSET_BUILDER_DECL(Texture)

#ifdef ENABLE_ASSET_BUILDING
XR_ASSET_BUILDER_BUILD_SIG(Texture)
{
  // parse image
  Image img;
  bool success = img.Parse(buffer.data, buffer.size);
  LTRACEIF(!success, ("%s: failed to parse image data.", rawNameExt));

  if (success)
  {
    uint32_t width = img.GetWidth();
    uint32_t height = img.GetHeight();

    // TOOD: support compressed textures.
    Gfx::TextureFormat  format;
    switch (img.GetBytesPerPixel())
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

    success = SerializeTexture(format, width, height, { img.GetPixelDataSize(), img.GetPixelData() }, data);
    LTRACEIF(!success, ("%s: failed to serialize texture.", rawNameExt));
  }
  return success;
}
#endif

uint32_t GetTextureFlags(uint32_t flags)
{
  uint32_t textureFlags = 0;
  if (CheckAllMaskBits(flags, Texture::WrapFlag))
  {
    textureFlags |= Gfx::F_TEXTURE_WRAP;
  }
  if (CheckAllMaskBits(flags, Texture::PointFilterFlag))
  {
    textureFlags |= Gfx::F_TEXTURE_FILTER_POINT;
  }
  if (CheckAllMaskBits(flags, Texture::MipMapsFlag))
  {
    textureFlags |= Gfx::F_TEXTURE_MIPMAPS;
  }
  if (CheckAllMaskBits(flags, Texture::SrgbFlag))
  {
    textureFlags |= Gfx::F_TEXTURE_SRGB;
  }
  return textureFlags;
}

bool HasAlphaHelper(Gfx::TextureFormat format)
{
  bool hasAlpha = false;
  switch (format)
  {
  case Gfx::TextureFormat::BGRA8:
  case Gfx::TextureFormat::RGBA8:
    hasAlpha = true;
    break;

  default:
    break;
  }
  return hasAlpha;
}

}

//==============================================================================
void Texture::RegisterSamplerUniform(char const* name, uint32_t textureStage)
{
  struct HandleHolder: Linked<HandleHolder>
  {
    Gfx::UniformHandle value;

    HandleHolder(char const* name, uint32_t stage)
    : Linked<HandleHolder>(*this),
      value{ Gfx::CreateUniform(name, Gfx::UniformType::Int1) }
    {
      Gfx::SetUniform(value, 1, &stage);

      static bool initialized = false;
      if (!initialized)
      {
        Gfx::RegisterShutdownCallback([](void*, void* ) {
          ForEach([](HandleHolder& hh) {
            Gfx::Release(hh.value);
            delete &hh;
          });
          initialized = false;  // Gfx was torn down.
        }, nullptr);
        initialized = true;
      }
    }
  };
  new HandleHolder(name, textureStage); // Let Linked<> take care of it.
}

//==============================================================================
Texture::Ptr Texture::FromHandle(Gfx::TextureHandle handle)
{
  XR_ASSERT(Texture, handle.IsValid());

  const auto info = Gfx::GetTextureInfo(handle);
  const bool isCube = CheckAllMaskBits(info.flags, Gfx::F_TEXTURE_CUBE);
  XR_ASSERTMSG(Texture, !isCube, ("2D texture handle required, %s given.", "cubemap"));
  const bool is3d = info.depth > 0;
  XR_ASSERTMSG(Texture, !is3d, ("2D texture handle required, %s given.", "3D texture"));

  Texture::Ptr texture;
  if (!(isCube || is3d))
  {
    texture.Reset(Create(0, UnmanagedFlag)->Cast<Texture>());
    // NOTE: need ReadyFlag to be able to Unload(), but it can't be set with Create();
    // NOTE: normally, synchronization should be used to access m_flags, but
    // since the Texture is just being created, there's no way anyone else is
    // accessing it.
    texture->m_flags |= ReadyFlag;
    texture->m_handle = handle;
    texture->m_width = info.width;
    texture->m_height = info.height;
    texture->m_hasAlpha = HasAlphaHelper(info.format);
  }

  return texture;
}

//==============================================================================
bool Texture::Upload(Gfx::TextureFormat format, uint32_t width, uint32_t height, Buffer buffer)
{
  OnUnload();

  const uint32_t flags = GetFlags();
  uint32_t textureFlags = GetTextureFlags(flags);

  // Create Gfx texture.
  m_width = width;
  m_height = height;
  m_hasAlpha = HasAlphaHelper(format);

  m_handle = Gfx::CreateTexture(format, width, height, 0, textureFlags, &buffer);
  bool success = m_handle.IsValid();
  if (success && CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
  {
    m_data.resize(sizeof(format) + sizeof(m_width) + sizeof(m_height) + sizeof(uint32_t) + buffer.size);
    class FixedStreamBuf: public std::streambuf
    {
    public:
      FixedStreamBuf(char* buffer, size_t size)
      {
        setp(buffer, buffer + size);
      }
    } fsb(reinterpret_cast<char*>(m_data.data()), m_data.size());

    std::ostream sourceBuffer(&fsb);
    success = SerializeTexture(format, width, height, buffer, sourceBuffer);
  }

  return success;
}

//==============================================================================
void Texture::Bind(uint32_t stage) const
{
  Gfx::SetTexture(m_handle, stage);
}

//==============================================================================
bool Texture::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  Gfx::TextureFormat format = Gfx::TextureFormat::kCount;
  uint32_t width = 0;
  uint32_t height = 0;
  bool success = reader.Read(format) && reader.Read(width) && reader.Read(height);
  LTRACEIF(!success, ("%s: failed to read texture dimensions.",
    m_debugPath.c_str()));

  uint32_t textureDataSize;
  uint8_t const* textureData;
  if (success) // read texture flags
  {
    // Create Gfx texture.
    textureData = reader.ReadBytesWithSize<uint32_t>(textureDataSize);
    success = textureData != nullptr;
    LTRACEIF(!success, ("%s: failed to read texel data.", m_debugPath.c_str()));
  }

  uint32_t flags;
  if (success)
  {
    flags = GetFlags();
    uint32_t textureFlags = GetTextureFlags(flags);

    Buffer textureBuffer = { textureDataSize, textureData };
    m_handle = Gfx::CreateTexture(format, width, height, 0, textureFlags, &textureBuffer);
    success = m_handle.IsValid();
    LTRACEIF(!success, ("%s: failed to create texture.", m_debugPath.c_str()));
  }

  if (success)
  {
    m_width = width;
    m_height = height;
    m_hasAlpha = HasAlphaHelper(format);

    if (CheckAllMaskBits(flags, KeepSourceDataFlag))
    {
      m_data.assign(buffer.data, buffer.data + buffer.size);
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

  Gfx::Release(m_handle);
  m_handle.Invalidate();

  std::vector<uint8_t>().swap(m_data);
}

} // XR
