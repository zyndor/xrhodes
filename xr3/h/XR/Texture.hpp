#ifndef XR_TEXTURE_HPP
#define XR_TEXTURE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Asset.hpp"
#include "Gfx.hpp"
#include "XR/Hash.hpp"

namespace XR
{

//==============================================================================
class Texture: public Asset
{
public:
  XR_ASSET_DECL(Texture);

  // types
  enum Flags : FlagType
  {
    WrapFlag = XR_MASK_ID(FlagType, 8),
    PointFilterFlag = XR_MASK_ID(FlagType, 9),
    MipMapsFlag = XR_MASK_ID(FlagType, 10),
    SrgbFlag = XR_MASK_ID(FlagType, 11),
  };

  // static
  ///@brief Convenience function to register a sampler uniform with the given
  /// @a name, for the given @a textureStage. This is assumed to not change
  /// value, and the handle is therefore not returned; it's cleaned up when
  /// Gfx::Exit()s.
  static void RegisterSamplerUniform(char const* name, uint32_t textureStage);

  ///@brief Attempts to create a texture from the given @a handle.
  ///@note It must be a valid handle to a 2D texture.
  static Texture::Ptr FromHandle(Gfx::TextureHandle handle);

  // general
  ///@return The number of texels the texture has horizontally at the highest
  /// mipmap level.
  int GetWidth() const;

  ///@return The number of texels the texture has vertically at the highest
  /// mipmap level.
  int GetHeight() const;

  ///@return Whether the texture has an alpha channel.
  bool HasAlpha() const;

  ///@brief Creates / updates a handle with one that's for a texture
  /// created with the given data.
  ///@note Must be called from the render thread.
  bool Upload(Gfx::TextureFormat format, uint32_t width, uint32_t height,
    Buffer buffer);

  ///@brief Binds texture to its target, for the given texture @a stage.
  ///@note Must be called from the render thread.
  void Bind(uint32_t stage) const;

private:
  // data
  uint32_t m_width = 0;
  uint32_t m_height = 0;
  bool m_hasAlpha = false;

  Gfx::TextureHandle  m_handle;
  std::vector<uint8_t> m_data; // If KeepSourceDataFlag is set.

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

//==============================================================================
inline
int Texture::GetWidth() const
{
	return m_width;
}

//==============================================================================
inline
int Texture::GetHeight() const
{
	return m_height;
}

//==============================================================================
inline
bool Texture::HasAlpha() const
{
	return m_hasAlpha;
}


} // XR

#endif // XR_TEXTURE_HPP
