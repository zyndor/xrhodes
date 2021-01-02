#ifndef XR_TEXTURE_HPP
#define XR_TEXTURE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Asset.hpp"
#include "Gfx.hpp"

namespace xr
{

//==============================================================================
///@brief A texture asset, which can be a 2D texture or a cube map. Textures may
/// be built from .png or .tga images, or a .tex file, which is a XON format with
/// the following properties:<br />
/// flags: options for creating the Texture. The supported values are the following
/// (all mapped to a Gfx::F_TEXTURE_* value): wrap, nearest, srgb, mipmapNearest,
/// mipmap, cube.<br />
/// - 'cube' will be inferred from keying paths to one of the sides (see below);<br />
/// - 'mipmap' will be inferred from defining mipmap levels (see below);<br />
/// paths: path to the image files that serve as the mipmap levels or faces. The
/// following configurations are supported:<br />
/// - single value: 2d texture with no manual mipmap;<br />
/// - object: either mipmap levels for 2d texture, OR cube map, if values are keyed
///   to '{+/-}{x/y/z}' OR the 'cube' flag was specified (in the latter case, there
///   must be exactly six entries); cube maps' mipmap levels may be defined by having
///   the individual entries as objects themselves, with the list of mipmap images;
///   (Note that this is transposed from the per level / per faces order that Gfx
///   is expecting them.)
///@note Mipmaps need to be defined from highest (level 0) to lowest resolution.
///@note The format of all mipmap levels must match.
///@note The format and size of all top level textures (in cube maps) must match.
///@note Individual images may specify flags in their filenames with the $ operator,
/// i.e. wooden_floor$mipmap$wrap.png. No spaces allowed between the $ and the id.
class Texture: public Asset
{
public:
  XR_ASSET_DECL(Texture)

  // types
#ifdef XR_COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
  enum [[deprecated("Use filename flags or .tex")]] Flags : FlagType
  {
    WrapFlag = FirstUserFlag,
    PointFilterFlag = FirstUserFlag << 1,
    MipMapsFlag = FirstUserFlag << 2,
    SrgbFlag = FirstUserFlag << 3,
  };
#ifdef XR_COMPILER_GCC
#pragma GCC diagnostic pop
#endif

  // static
  ///@brief Convenience function to register a sampler uniform with the given
  /// @a name, for the given @a textureStage. This is assumed to not change
  /// value, and the handle is therefore not returned; it's cleaned up when
  /// Gfx is Shutdown().
  static void RegisterSamplerUniform(char const* name, uint32_t textureStage);

  ///@brief Creates a Texture from the given @a handle. Only If the handle was
  /// valid, and is for a 2D texture, the ownership of it is transferred, to the
  /// Texture. The resulting Texture will be unmanaged.
  static Texture::Ptr FromHandle(Gfx::TextureHandle handle);

  // general
  ///@return The number of texels the texture has horizontally at the highest
  /// mipmap level.
  uint16_t GetWidth() const;

  ///@return The number of texels the texture has vertically at the highest
  /// mipmap level.
  uint16_t GetHeight() const;

  ///@return Whether the texture has an alpha channel.
  bool HasAlpha() const;

  ///@brief Creates / updates a handle with one that's for a texture
  /// created with the given data.
  ///@note Must be called from the render thread.
  bool Upload(Gfx::TextureFormat format, uint16_t width, uint16_t height,
    Buffer buffer);

  ///@brief Creates / updates a handle with one that's for a texture
  /// created with the given data.
  ///@note Must be called from the render thread.
  bool Upload(Gfx::TextureFormat format, uint16_t width, uint16_t height,
    Gfx::FlagType createFlags, uint32_t numBuffers, Buffer const* buffers);

  ///@brief Binds texture to its target, for the given texture @a stage.
  ///@note Must be called from the render thread.
  void Bind(uint32_t stage) const;

private:
  // data
  uint16_t m_width = 0;
  uint16_t m_height = 0;
  bool m_hasAlpha = false;

  Gfx::TextureHandle  m_handle;
  std::vector<uint8_t> m_data; // If KeepSourceDataFlag is set.

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

//==============================================================================
inline
uint16_t Texture::GetWidth() const
{
	return m_width;
}

//==============================================================================
inline
uint16_t Texture::GetHeight() const
{
	return m_height;
}

//==============================================================================
inline
bool Texture::HasAlpha() const
{
	return m_hasAlpha;
}


} // xr

#endif // XR_TEXTURE_HPP
