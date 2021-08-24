//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/TexturePack.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/utility/Hash.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "xr/utils.hpp"
#include "xr/strings/FilePath.hpp"
#include "xr/io/streamutils.hpp"
#include "xr/debug.hpp"
#include "tinyxml2.h"
#endif

#define LTRACE(format) XR_TRACE(TexturePack, format)

namespace xr
{

XR_WARNINGS_IGNORE_DEPRECATION

//==============================================================================
XR_ASSET_DEF(TexturePack, "xtpk", 3, "xtp")

using NumSpritesType = uint32_t;
using SpriteNameHashType = uint32_t;

#ifdef ENABLE_ASSET_BUILDING
namespace
{

enum
{
  TAG_IMAGE_PATH,
  TAG_SHADER_PATH,
  TAG_WIDTH,
  TAG_HEIGHT,
  TAG_SPRITE,
  TAG_NAME,
  TAG_X,
  TAG_Y,
  TAG_W,
  TAG_H,
  TAG_OFFSET_X,
  TAG_OFFSET_Y,
  TAG_ORIGINAL_W,
  TAG_ORIGINAL_H,
  TAG_ROTATED
};

static const char* const kTags[] =
{
  "imagePath",
  "shaderPath",
  "width",
  "height",
  "sprite",
  "n",
  "x",
  "y",
  "w",
  "h",
  "oX",
  "oY",
  "oW",
  "oH",
  "r"
};

XR_ASSET_BUILDER_DECL(TexturePack)

XR_ASSET_BUILDER_BUILD_SIG(TexturePack)
{
  // load xml
  tinyxml2::XMLDocument doc;
  auto xmlError = doc.Parse(buffer.As<char const>(), buffer.size);
  if (xmlError != tinyxml2::XML_SUCCESS)
  {
    LTRACE(("%s: failed to parse xml, code: %d", rawNameExt, xmlError));
    return false;
  }

  tinyxml2::XMLElement* elem = doc.RootElement();
  if (!elem)
  {
    LTRACE(("%s: no root element found.", rawNameExt));
    return false;
  }

  // Base texture name
  const char* textureName = elem->Attribute(kTags[TAG_IMAGE_PATH]);
  if (!textureName)
  {
    LTRACE(("%s: '%s' is a required attribute.", rawNameExt,
      kTags[TAG_IMAGE_PATH]));
    return false;
  }

  dependencies.push_back(textureName);

  Asset::HashType textureHash = Asset::Manager::HashPath(dependencies.back());
  if (!WriteBinaryStream(textureHash, data))
  {
    LTRACE(("%s: failed to write texture hash.", rawNameExt));
    return false;
  }

  // Base texture size.
  int texWidth = 0;
  if (elem->QueryIntAttribute(kTags[TAG_WIDTH], &texWidth) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
  {
    LTRACE(("%s: invalid value for %s: %s", rawNameExt,
      kTags[TAG_WIDTH], elem->Attribute(kTags[TAG_WIDTH])));
    return false;
  }

  int texHeight = 0;
  if (elem->QueryIntAttribute(kTags[TAG_HEIGHT], &texHeight) == tinyxml2::XML_WRONG_ATTRIBUTE_TYPE)
  {
    LTRACE(("%s: invalid value for %s: %s", rawNameExt,
      kTags[TAG_HEIGHT], elem->Attribute(kTags[TAG_HEIGHT])));
    return false;
  }

  // Number of Sprites.
  NumSpritesType numSprites = 0;
  auto counter = elem->FirstChildElement("sprite");
  while (counter)
  {
    ++numSprites;
    counter = counter->NextSiblingElement("sprite");
  }

  if (!WriteBinaryStream(numSprites, data))
  {
    LTRACE(("%s: failed to write sprite count.", rawNameExt));
    return false;
  }

  // Sprite data
  elem = elem->FirstChildElement(kTags[TAG_SPRITE]);

  int x, y; // position of top left corner on sprite sheet
  int w, h; // actual size on sprite sheet. This will inform the halfSize. Needs swapping if the sprite is rotated.
  numSprites = 0;
  while (elem)
  {
    // Amount of translation padding left of and above sprite, or bottom and left, if sprite is rotated.
    int xOffs = 0;
    int yOffs = 0;
    // Original width / height, if trimmed. Accounts for rotation. Includes all padding.
    // For un-trimmed sprites, wOrig and hOrig are their w / h. If the sprite is rotated, we need to swap those.
    int wOrig = 0;
    int hOrig = 0;

    // Get sprite attributes.
    const char* spriteName = elem->Attribute(kTags[TAG_NAME]);

    if (!(spriteName &&
      elem->QueryIntAttribute(kTags[TAG_W], &w) == tinyxml2::XML_SUCCESS &&
      elem->QueryIntAttribute(kTags[TAG_H], &h) == tinyxml2::XML_SUCCESS &&
      elem->QueryIntAttribute(kTags[TAG_X], &x) == tinyxml2::XML_SUCCESS &&
      elem->QueryIntAttribute(kTags[TAG_Y], &y) == tinyxml2::XML_SUCCESS &&
      elem->QueryIntAttribute(kTags[TAG_ORIGINAL_W], &wOrig) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
      elem->QueryIntAttribute(kTags[TAG_ORIGINAL_H], &hOrig) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
      elem->QueryIntAttribute(kTags[TAG_OFFSET_X], &xOffs) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
      elem->QueryIntAttribute(kTags[TAG_OFFSET_Y], &yOffs) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE))
    {
      LTRACE(("%s: definition of sprite %d missing a required element.",
        rawNameExt, numSprites));
      return false;
    }

    // Get name (of original image), strip extension.
    auto spriteNameLen = strlen(spriteName);
    if (auto period = strrchr(spriteName, '.'))
    {
      spriteNameLen = period - spriteName;
    }

    // Calculate UVs - convert from bitmap-space to texture-space (bottom-left based).
    // TODO: consider support for non-openGL renderers where texture-space might be top-left based.
    auto texHeightLessY = texHeight - y;
    AABB  uvs =
    {
      float(x) / texWidth,
      float(texHeightLessY) / texHeight,
      float(x + w) / texWidth,
      float(texHeightLessY - h) / texHeight,
    };

    // Get UV rotation attribute.
    const char* rotationAttrib = elem->Attribute(kTags[TAG_ROTATED]);
    const bool  isRotated = rotationAttrib != nullptr && strlen(rotationAttrib) > 0 &&
      rotationAttrib[0] == 'y';

    // Calculate UVs and vertex positions.
    Sprite sprite;
    if (isRotated)
    {
      sprite.SetUVsRotatedProportional(uvs, texHeight, texWidth);
    }
    else
    {
      sprite.SetUVsProportional(uvs, texWidth, texHeight);
    }

    // Calculate offset for padding.
    // If wOffs / hOffs isn't set, then use sprite width / height, depending on whether it's rotated.
    if (wOrig == 0)
    {
      wOrig = isRotated ? h : w;
    }

    if (hOrig == 0)
    {
      hOrig = isRotated ? w : h;
    }

    // NOTE: y conversion (negation) applies since we're moving from bitmap space to model space.
    // TODO: consider support for non-openGL renderers where model-space might have negative Y.
    Vector2 offset;
    if (isRotated)
    {
      std::swap(w, h);
    }
    offset.x = xOffs - (wOrig - w) * .5f;
    offset.y = (hOrig - h) * .5f - yOffs;
    sprite.AddOffset(offset.x, offset.y, true);

    // Calculate halfSize.
    sprite.SetHalfSize(wOrig * .5f, hOrig * .5f, false);

    // Write sprite
    SpriteNameHashType spriteHash = Hash::String32(spriteName, spriteNameLen);
    if (!WriteBinaryStream(spriteHash, data) && WriteBinaryStream(sprite, data))
    {
      LTRACE(("%s: failed to write sprite %d.", rawNameExt, numSprites));
      return false;
    }

    elem = elem->NextSiblingElement(kTags[TAG_SPRITE]);
    ++numSprites;
  }

  return true;
}

}
#endif

//==============================================================================
Sprite* TexturePack::Get(char const* name)
{
  XR_ASSERT(TexturePack, name != nullptr);
  Sprite* pSprite = Get(Hash::String32(name));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get(char const* name) const
{
  XR_ASSERT(TexturePack, name != nullptr);
  const Sprite* pSprite = Get(Hash::String32(name));
  return pSprite;
}

//==============================================================================
void  TexturePack::ScaleSprites(float x)
{
  ScaleSprites(x, x);
}

//==============================================================================
void  TexturePack::ScaleSprites(float x, float y)
{
  for (SpriteMap::iterator i0(m_sprites.begin()), i1(m_sprites.end()); i0 != i1; ++i0)
  {
    i0->second.Scale(x, y);
  }
}

//==============================================================================
void TexturePack::Clear()
{
  m_sprites.clear();
}

//==============================================================================
bool TexturePack::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  HashType textureHash;
  if (!reader.Read(textureHash))
  {
    LTRACE(("%s: failed to read texture id.", m_debugPath.c_str()));
    return false;
  }

  m_texture = Manager::Find(Descriptor<Texture>(textureHash));
  if (!m_texture)
  {
    LTRACE(("%s: failed to retrieve texture.", m_debugPath.c_str()));
    return false;
  }

  NumSpritesType numSprites;
  if (!reader.Read(numSprites))
  {
    LTRACE(("%s: failed to read sprite count.", m_debugPath.c_str()));
    return false;
  }

  SpriteNameHashType spriteHash;
  Sprite sprite;
  NumSpritesType i = 0;
  while (i < numSprites)
  {
    if (reader.Read(spriteHash) && reader.Read(sprite))
    {
      Sprite& spr = m_sprites[spriteHash];
      spr = sprite;
      ++i;
    }
    else
    {
      LTRACE(("%s: failed to read sprite %d.", m_debugPath.c_str(), i));
      return false;
    }
  }

  return true;
}

//==============================================================================
void TexturePack::OnUnload()
{
  m_texture.Reset(nullptr);
  m_sprites.clear();
}

} // xr
