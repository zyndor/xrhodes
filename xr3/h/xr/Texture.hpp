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
#include "xr/Hash.hpp"

namespace xr
{

//==============================================================================
///@brief A 2D texture asset.
class Texture: public Asset
{
public:
  XR_ASSET_DECL(Texture);

  // types
  enum Flags : FlagType
  {
    WrapFlag = FirstUserFlag,
    PointFilterFlag = FirstUserFlag << 1,
    MipMapsFlag = FirstUserFlag << 2,
    SrgbFlag = FirstUserFlag << 3,
  };

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
