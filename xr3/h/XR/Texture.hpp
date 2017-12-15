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

  enum Flags : FlagType
  {
    WrapFlag = XR_MASK_ID(FlagType, 8),
    PointFilterFlag = XR_MASK_ID(FlagType, 9),
    MipMapsFlag = XR_MASK_ID(FlagType, 10),
    SrgbFlag = XR_MASK_ID(FlagType, 11),
  };

  // general
  ///@return The number of texels the texture has horizontally at the highest
  /// mipmap level.
  int     GetWidth() const;

  ///@return The number of texels the texture has vertically at the highest
  /// mipmap level.
  int     GetHeight() const;

  ///@return Whether the texture has an alpha channel.
  bool    HasAlpha() const;
  
  ///@brief Binds texture to its target, for the given texture @a stage.
  void Bind(uint32_t stage) const;

  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
  
private:
  // data
  uint32_t m_width = 0;
  uint32_t m_height = 0;
  bool m_hasAlpha = false;

  Gfx::TextureHandle  m_handle;
  std::vector<uint8_t> m_data; // If KeepSourceDataFlag is set.
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