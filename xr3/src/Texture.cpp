//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Texture.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/io/FixedStreamBuf.hpp"
#include "xr/io/streamutils.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "ParseAssetOptions.hpp"
#include "Ktx.hpp"
#include "xr/Image.hpp"
#include "xr/FileWriter.hpp"
#include "xr/strings/ParserCore.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include <unordered_map>
#endif

#define LTRACE(format) XR_TRACE(Texture, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Texture, "xtex", 4, "png;tga;ktx;tex")

//==============================================================================
namespace {

using Size = uint32_t;  // serialized size
using TextureSize = decltype(Gfx::TextureInfo::width);
static_assert(std::is_same<decltype(Gfx::TextureInfo::width), decltype(Gfx::TextureInfo::height)>::value, "");

struct TextureHeader
{
  Gfx::TextureFormat format;
  TextureSize width;
  TextureSize height;
  Gfx::FlagType createFlags;
  Size numBuffers;  // 8 bits used
};

const Size kMaxBuffers = std::numeric_limits<uint8_t>::max();

bool SerializeTexture(TextureHeader const& header, Buffer const* buffers,
  std::ostream& dataOut)
{
  bool ok = WriteBinaryStream(header, dataOut);
  auto iEnd = buffers + header.numBuffers;
  while (ok && buffers != iEnd)
  {
    ok = WriteRangeBinaryStream(buffers->data, buffers->data + buffers->size, dataOut);
    ++buffers;
  }
  return ok;
}

#ifdef ENABLE_ASSET_BUILDING
XR_ASSET_BUILDER_DECL(Texture)

using FormatHandler = bool(*)(const char* rawNameExt, Buffer buffer, std::ostream& data);

const std::unordered_map<std::string, uint16_t> kFlags{
  { "wrap", uint16_t(Gfx::F_TEXTURE_WRAP) },
  { "filterPoint", uint16_t(Gfx::F_TEXTURE_FILTER_POINT) },
  { "srgb", uint16_t(Gfx::F_TEXTURE_SRGB) },
  { "mipmapNearest", uint16_t(Gfx::F_TEXTURE_MIPMAP_NEAREST) },
  { "mipmap", uint16_t(Gfx::F_TEXTURE_MIPMAP) },
  { "cube", uint16_t(Gfx::F_TEXTURE_CUBE) },
};

const std::unordered_map<std::string, size_t> kCubeFaces{
  { "+x", 0 },
  { "-x", 1 },
  { "+y", 2 },
  { "-y", 3 },
  { "+z", 4 },
  { "-z", 5 }
};

Gfx::FlagType ParseTextureOptions(char const* nameExt)
{
  Gfx::FlagType createFlags = Gfx::F_TEXTURE_NONE;
  ParseAssetOptions(nameExt, [nameExt, &createFlags](const char* option) {
    auto iFind = kFlags.find(option);
    bool result = iFind != kFlags.end();
    if (result)
    {
      createFlags |= iFind->second;
    }
    else
    {
      LTRACE(("%s: Unsupported option '%s' ignored.", nameExt, option));
    }
    return result;
  });
  return createFlags;
}

bool  ProcessImage(const char* rawNameExt, Buffer buffer, std::ostream& data)
{
  Gfx::FlagType createFlags = ParseTextureOptions(rawNameExt);

  // parse image
  Image img;
  if (!img.Parse(buffer.data, buffer.size))
  {
    LTRACE(("%s: failed to parse image data.", rawNameExt));
    return false;
  }

  const uint32_t kMaxWidth = std::numeric_limits<TextureSize>::max();
  const uint32_t kMaxHeight = std::numeric_limits<TextureSize>::max();
  if (!(img.GetWidth() <= kMaxWidth && img.GetHeight() <= kMaxHeight))
  {
    LTRACE(("%s: image dimension excessive: %u x %u; 16bits maximum.",
      rawNameExt, img.GetWidth(), img.GetHeight()));
    return false;
  }

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
  default:
    LTRACE(("%s: invalid bpp: %d.", rawNameExt, img.GetBytesPerPixel()));
    return false;
  }

  TextureHeader header { format, static_cast<TextureSize>(img.GetWidth()),
    static_cast<TextureSize>(img.GetHeight()), createFlags, 1 };
  Buffer pixelBuffer { img.GetPixelDataSize(), img.GetPixelData() };
  if (!SerializeTexture(header, &pixelBuffer, data))
  {
    LTRACE(("%s: failed to serialize texture.", rawNameExt));
    return false;
  }
  return true;
}

bool ProcessKtx(const char* rawNameExt, Buffer buffer, std::ostream& data)
{
  Gfx::FlagType createFlags = ParseTextureOptions(rawNameExt);

  Ktx ktx;
  if (!ktx.Parse(buffer.size, buffer.data))
  {
    LTRACE(("%s: failed to parse image data.", rawNameExt));
    return false;
  }

  createFlags |= Gfx::F_TEXTURE_CUBE * (ktx.mFaceCount > 1);
  createFlags |= Gfx::F_TEXTURE_MIPMAP * (ktx.mMipLevelCount > 1);
  TextureHeader header
  {
    ktx.mFormat,
    ktx.mWidthPx,
    ktx.mHeightPx,
    createFlags,
    static_cast<uint32_t>(ktx.mImages.size())
  };
  if (!SerializeTexture(header, ktx.mImages.data(), data))
  {
    LTRACE(("%s: failed to serialize texture.", rawNameExt));
    return false;
  }
  return true;
}

uint16_t GetFlag(XonValue const& xonValue, const char* rawNameExt)
{
  auto strValue = xonValue.GetString();
  auto iFind = kFlags.find(strValue);
  if (iFind != kFlags.end())
  {
    return iFind->second;
  }

  LTRACE(("%s: '%s' not a valid flag; ignored.", rawNameExt, strValue));
  return 0;
}

/*
  {
    paths: {
      +x: skybox_right.png,
      -z: skybox_front.png,
      -x: skybox_left.png,
      +y: skybox_above.png
    }, # OR
    paths: {
      +x: { skybox_right_mip0.png, skybox_right_mip1.png, #etc.
      }, # etc.
    }, # OR
    paths: {  # 2d texture
      wooden_floor.png
    }, # OR
    paths: {  # 2d texture with manual mipmap, unless 'cube' flag is specified.
      wooden_floor_mip0.png,
      wooden_floor_mip1.png,
      # etc.
    },
    flags: { wrap, nearest, mipmap }
  }
*/
bool  ProcessTex(const char* rawNameExt, Buffer buffer, std::ostream& data)
{
  XonParser::State xonState;
  std::unique_ptr<XonObject> xonRoot(XonBuildTree(buffer.As<char const>(),
    buffer.size, &xonState));
  if (!xonRoot)
  {
    LTRACE(("%s: failed to parse XON somewhere around row %d, column %d", rawNameExt,
      xonState.row, xonState.column));
    return false;
  }

  // Get texture flags
  uint16_t flags = Gfx::F_TEXTURE_NONE; // Gfx Texture flags don't take more than 8 bits.
  if (auto xonFlags = xonRoot->TryGet("flags"))
  {
    if (xonFlags->GetType() == XonEntity::Type::Value)
    {
      flags = GetFlag(xonFlags->ToValue(), rawNameExt);
    }
    else
    {
      auto& xonFlagsObj = xonFlags->ToObject();
      for (size_t i = 0, n = xonFlagsObj.GetNumElements(); i < n; ++i)
      {
        auto& xonFlag = xonFlagsObj[i];
        if (xonFlag.GetType() == XonEntity::Type::Value)
        {
          flags |= GetFlag(xonFlag.ToValue(), rawNameExt);
        }
        else
        {
          LTRACE(("%s: XON value required for flags[%d]; ignored.", rawNameExt, i));
        }
      }
    }
  }

  // Process image paths.
  auto eXonPaths = xonRoot->TryGet("paths");
  if (!eXonPaths)
  {
    LTRACE(("%s: missing 'paths' definition.", rawNameExt));
    return false;
  }

  // Get sources of paths. If eXonPaths is an object, look for cubemap keys.
  // If no cubemap keys were found, register all elements (or exactly six if
  // clearly a cubemap).
  bool isCube = CheckAllMaskBits(flags, Gfx::F_TEXTURE_CUBE);

  std::vector<XonEntity*> xonFaces;
  XonEntity* xonFirstFace = eXonPaths;  // default to the 'paths' element itself.

  auto xonPaths = eXonPaths->GetType() == XonEntity::Type::Object ?
    &eXonPaths->ToObject() : nullptr;
  if (xonPaths)
  {
    if (xonPaths->GetNumElements() == 0)
    {
      LTRACE(("%s: no images specified for texture.", rawNameExt));
      return false;
    }

    // First face defined will determine the number of mipmap levels.
    xonFirstFace = &(*xonPaths)[0];

    for (auto& i: kCubeFaces) // look for cubemap keys
    {
      if (auto eXonFace = xonPaths->TryGet(i.first))
      {
        xonFaces.resize(kCubeFaces.size(), nullptr);
        xonFaces[i.second] = eXonFace;
      }
    }

    if (!xonFaces.empty())  // tagging a face makes the texture a cubemap.
    {
      isCube = true;
      flags |= Gfx::F_TEXTURE_CUBE;
    }
    else
    {
      size_t n = xonPaths->GetNumElements();
      if (isCube)
      {
        if (n != kCubeFaces.size())
        {
          LTRACE(("%s: cubemap definitions require either tagging faces, or providing"
            " a definition for exactly 6 of them (%d given).", rawNameExt, n));
          return false;
        }
      }
      else if (xonFirstFace->Is(XonEntity::Type::Object) && n > 1)
      {
        LTRACE(("%s: ignoring %d additional faces on 2D texture.", rawNameExt, n - 1));
        n = 1;
      }

      xonFaces.reserve(n);
      for (size_t i = 0; i < n; ++i)
      {
        xonFaces.push_back(&(*xonPaths)[i]);
      }
    }
  }
  else
  {
    xonFaces.push_back(eXonPaths);
  }

  // Figure out number of mip levels and faces.
  XR_ASSERT(Texture, xonFirstFace->Is(XonEntity::Type::Value) ||
    xonFirstFace->ToObject().GetNumElements() < std::numeric_limits<uint32_t>::max());
  const auto numMipLevels = xonFirstFace->Is(XonEntity::Type::Object) ?
    xonFirstFace->ToObject().GetNumElements() : 1;
  if (numMipLevels > 1) // turn on mipmapping if mipmaps were specified.
  {
    flags |= Gfx::F_TEXTURE_MIPMAP;
  }

  const auto numFaces = xonFaces.size();
  const auto numBuffers = numMipLevels * numFaces;

  // Process xon entities, get layers where possible.
  std::vector<char const*> imagePaths;
  imagePaths.reserve(numBuffers);
  for (auto i: xonFaces)
  {
    const size_t oldSize = imagePaths.size();
    if (i != nullptr) // not a missing keyed cube face
    {
      if (i->Is(XonEntity::Type::Value))
      {
        imagePaths.push_back(i->ToValue().GetString());
      }
      else
      {
        auto& xonLayer = i->ToObject();
        for (size_t j0 = 0, j1 = xonLayer.GetNumElements(); j0 < j1; ++j0)
        {
          auto& eXonPath = xonLayer[j0];
          if (eXonPath.Is(XonEntity::Type::Object))
          {
            LTRACE(("%s: unexpected inner object on paths[%d][%d]; ignored.", rawNameExt,
              &i - xonFaces.data(), j0));
          }
          else
          {
            imagePaths.push_back(eXonPath.ToValue().GetString());
          }
        }
      }
    }

    // fill remaining layers with empty paths.
    imagePaths.insert(imagePaths.end(), numMipLevels - (imagePaths.size() - oldSize),
      "");
  }

  // Load images.
  std::vector<Image>  images;
  images.resize(imagePaths.size());

  std::vector<Buffer> imageBuffers;
  imageBuffers.resize(imagePaths.size());
  for (size_t i = 0; i < imagePaths.size(); ++i)
  {
    FilePath path(imagePaths[i]);
    auto& image = images[i];
    if (!path.empty() && !image.Load(path))
    {
      LTRACE(("%s: failed to load '%s'.", rawNameExt, path.c_str()));
      return false;
    }
    else if (path.empty() && i % numMipLevels == 0)
    {
      LTRACE(("%s: must provide image for top mipmap level.'.", rawNameExt));
      return false;
    }

    if (i > 0)
    {
      if (!(image.GetWidth() == 0 && image.GetHeight() == 0))
      {
        // If top level image, dimensions must match original
        // otherwise dimensions must be half of previous image
        const auto level = i % numMipLevels;
        uint32_t divisor = 1 << level;
        uint32_t expectWidth = images[0].GetWidth() / divisor;
        uint32_t expectHeight = images[0].GetHeight() / divisor;
        if (!(image.GetWidth() == expectWidth && image.GetHeight() == expectHeight))
        {
          LTRACE(("%s: size mismatch in '%s'; expected: %d x %d, got: %d x %d.", rawNameExt,
            path.c_str(), expectWidth, expectHeight, image.GetWidth(), image.GetHeight()));
        }

        if (image.GetBytesPerPixel() != images[0].GetBytesPerPixel())
        {
          LTRACE(("%s: format mismatch in '%s': expected %d bpp, got: %d", rawNameExt,
            path.c_str(), images[0].GetBytesPerPixel(), image.GetBytesPerPixel()));
          return false;
        }
      }
    }
    else // first image -- dimensions must be <= 64K
    {
      const auto kMaxWidth = std::numeric_limits<TextureSize>::max();
      const auto kMaxHeight = std::numeric_limits<TextureSize>::max();
      if (image.GetWidth() > kMaxWidth || image.GetHeight() > kMaxHeight)
      {
        LTRACE(("%s: image dimension excessive: %u x %u; 16bits maximum.",
          rawNameExt, image.GetWidth(), image.GetHeight()));
        return false;
      }
    }

    // insert buffers transposed.
    auto iT = (i % numMipLevels) * numFaces + i / numMipLevels;
    imageBuffers[iT] = { images[i].GetPixelDataSize(), images[i].GetPixelData() };
  }

  Gfx::TextureFormat  format;
  switch (images[0].GetBytesPerPixel())
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
  default:
    LTRACE(("%s: invalid bpp: %d.", rawNameExt, images[0].GetBytesPerPixel()));
    return false;
  }

  TextureHeader header {
    format,
    static_cast<TextureSize>(images[0].GetWidth()),
    static_cast<TextureSize>(images[0].GetHeight()),
    flags,
    static_cast<uint32_t>(imageBuffers.size())
  };
  return SerializeTexture(header, imageBuffers.data(), data);
}

const std::unordered_map<std::string, FormatHandler> kFormatHandlers{
  { "png", ProcessImage },
  { "tga", ProcessImage },
  { "ktx", ProcessKtx },
  { "tex", ProcessTex },
};

XR_ASSET_BUILDER_BUILD_SIG(Texture)
{
  std::string extension = [](char const* nameExt) {
    auto ext = FilePath(nameExt).GetExt();
    return std::string(ext, strcspn(ext, Asset::kOptionDelimiter));
  }(rawNameExt);
  auto iFind = kFormatHandlers.find(extension.c_str());
  XR_ASSERT(Texture, iFind != kFormatHandlers.end());
  return iFind->second(rawNameExt, buffer, data);
}
#endif

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

} // nonamespace

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
        Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>([](void*) {
          ForEach([](HandleHolder& hh) {
            Gfx::Release(hh.value);
            delete &hh;
          });
          initialized = false;  // Gfx was torn down.
        }, nullptr));
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
  Texture::Ptr texture(Create(0, UnmanagedFlag));
  // NOTE: need ReadyFlag to be able to Unload(), but it can't be set with Create();
  // NOTE: normally, synchronization should be used to access m_flags, but
  // since the Texture is just being created, there's no way anyone else is
  // accessing it.
  texture->m_flags |= ReadyFlag;
  texture->m_handle = handle;
  texture->m_width = info.width;
  texture->m_height = info.height;
  texture->m_hasAlpha = HasAlphaHelper(info.format);

  return texture;
}

//==============================================================================
bool Texture::Upload(Gfx::TextureFormat format, uint16_t width, uint16_t height,
  Buffer buffer)
{
  return Upload(format, width, height, Gfx::F_TEXTURE_NONE, 1, &buffer);
}

//==============================================================================
bool Texture::Upload(Gfx::TextureFormat format, uint16_t width, uint16_t height,
  Gfx::FlagType createFlags, uint8_t numBuffers, Buffer const* buffers)
{
  OnUnload();

  XR_DEBUG_ONLY(const auto flags = GetFlags());
  XR_ASSERT(Texture, CheckAllMaskBits(flags, ErrorFlag) ||
    !CheckAnyMaskBits(flags, LoadingFlag | ProcessingFlag));

  // Create Gfx texture.
  m_width = width;
  m_height = height;
  m_hasAlpha = HasAlphaHelper(format);

  m_handle = Gfx::CreateTexture(format, width, height, 0, createFlags, buffers,
    numBuffers);
  bool success = m_handle.IsValid();
  if (success && CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
  {
    Size  sourceDataSize = sizeof(format) + sizeof(width) + sizeof(height) +
      sizeof(Gfx::FlagType) + sizeof(Size);
    for (uint32_t i = 0; i < numBuffers; ++i)
    {
      sourceDataSize += sizeof(Size) + static_cast<uint32_t>(buffers->size);
    }

    m_data.resize(sourceDataSize);

    FixedStreamBuf fsb(m_data.size(), reinterpret_cast<char*>(m_data.data()));
    std::ostream sourceStream(&fsb);

    TextureHeader header { format, width, height, createFlags, numBuffers };
    success = SerializeTexture(header, buffers, sourceStream);
  }

  OverrideFlags(PrivateMask, success ? ReadyFlag : (ProcessingFlag | ErrorFlag));

  return success;
}

//==============================================================================
void Texture::Bind(uint8_t stage) const
{
  Gfx::SetTexture(m_handle, stage);
}

//==============================================================================
bool Texture::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  TextureHeader header;
  if (!reader.Read(header))
  {
    LTRACE(("%s: failed to read texture header.", m_debugPath.c_str()));
    return false;
  }

  if (header.numBuffers > kMaxBuffers)
  {
    LTRACE(("%s: %d too many buffers. These are ignored.", header.numBuffers - kMaxBuffers));
    header.numBuffers = kMaxBuffers;
  }

  std::vector<Buffer> pixelBuffers(header.numBuffers);
  for (auto& b : pixelBuffers)
  {
    Size size;
    b.data = reader.ReadBytesWithSize(size);
    if (!b.data)
    {
      LTRACE(("%s: Failed to read texel data for buffer %d.", m_debugPath.c_str(),
        &b - pixelBuffers.data()));
      return false;
    }
    b.size = size;
  }

  Asset::FlagType flags = GetFlags();

#ifdef ENABLE_ASSET_BUILDING
  if (!CheckAllMaskBits(flags, DryRunFlag))
  {
#endif
  m_handle = Gfx::CreateTexture(header.format, header.width, header.height,
    0, header.createFlags, pixelBuffers.data(), uint8_t(header.numBuffers));
  if (!m_handle.IsValid())
  {
    LTRACE(("%s: failed to create texture.", m_debugPath.c_str()));
    return false;
  }
#ifdef ENABLE_ASSET_BUILDING
  }
#endif

  m_width = header.width;
  m_height = header.height;
  m_hasAlpha = HasAlphaHelper(header.format);

  if (CheckAllMaskBits(flags, KeepSourceDataFlag))
  {
    m_data.assign(buffer.data, buffer.data + buffer.size);
  }

  return true;
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
