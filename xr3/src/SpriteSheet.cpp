//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/SpriteSheet.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/utility/Hash.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "xr/utils.hpp"
#include "xr/strings/FilePath.hpp"
#include "xr/io/streamutils.hpp"
#include "xr/debug.hpp"
#include "tinyxml2.h"
#endif

#define LTRACE(format) XR_TRACE(SpriteSheet, format)
#define LTRACEIF(condition, format) XR_TRACEIF(SpriteSheet, condition, format)

namespace xr
{

XR_ASSET_DEF(SpriteSheet, "ssht", 1, ".sprites")

using ImagePathLenType = uint16_t;
using NumSpritesType = uint32_t;
using SpriteNameHashType = uint32_t;

#ifdef ENABLE_ASSET_BUILDING
namespace
{

enum
{
  TAG_IMAGE_PATH,
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


XR_ASSET_BUILDER_DECL(SpriteSheet)

XR_ASSET_BUILDER_BUILD_SIG(SpriteSheet)
{
  // load xml
  tinyxml2::XMLDocument doc;
  auto xmlError = doc.Parse(buffer.As<char const>(), buffer.size);
  bool success = xmlError == tinyxml2::XML_SUCCESS;
  LTRACEIF(!success, ("%s: failed to parse xml, code: %d", rawNameExt, xmlError));

  tinyxml2::XMLElement* elem = doc.RootElement();
  if (success)
  {
    success = elem != nullptr;
    LTRACEIF(!success, ("%s: no root element found.", rawNameExt));
  }

  // Image path
  const char* imagePath = nullptr;
  if (success)
  {
    imagePath = elem->Attribute(kTags[TAG_IMAGE_PATH]);
    success = imagePath != nullptr;
    LTRACEIF(!success, ("%s: '%s' is a required attribute.", rawNameExt,
      kTags[TAG_IMAGE_PATH]));
  }

  if (success)
  {
    size_t imagePathLen = strlen(imagePath);
    XR_ASSERT(SpriteSheet, imagePathLen < std::numeric_limits<ImagePathLenType>::max());
    success = WriteRangeBinaryStream<ImagePathLenType>(imagePath,
      imagePath + imagePathLen, data);
    LTRACEIF(!success, ("%s: failed to write image path.", rawNameExt));
  }

  // Base texture size.
  int texWidth = 0;
  if (success)
  {
    success = elem->QueryIntAttribute(kTags[TAG_WIDTH], &texWidth) !=
      tinyxml2::XML_WRONG_ATTRIBUTE_TYPE;
    LTRACEIF(!success, ("%s: invalid value for %s: %s", rawNameExt,
      kTags[TAG_WIDTH], elem->Attribute(kTags[TAG_WIDTH])));
  }

  int texHeight = 0;
  if (success)
  {
    success = elem->QueryIntAttribute(kTags[TAG_HEIGHT], &texHeight) !=
      tinyxml2::XML_WRONG_ATTRIBUTE_TYPE;
    LTRACEIF(!success, ("%s: invalid value for %s: %s", rawNameExt,
      kTags[TAG_HEIGHT], elem->Attribute(kTags[TAG_HEIGHT])));
  }

  // Number of Sprites.
  NumSpritesType numSprites = 0;
  if (success)
  {
    auto counter = elem->FirstChildElement(kTags[TAG_SPRITE]);
    while (counter)
    {
      ++numSprites;
      counter = counter->NextSiblingElement(kTags[TAG_SPRITE]);
    }

    success = WriteBinaryStream(numSprites, data);
    LTRACEIF(!success, ("%s: failed to write sprite count.", rawNameExt));
  }

  // Sprite data
  if (success)
  {
    SpriteSheet::SpriteVector sprites;
    sprites.reserve(numSprites);

    elem = elem->FirstChildElement(kTags[TAG_SPRITE]);

    HardString<256> buffer;
    int x, y; // position of top left corner on sprite sheet
    int w, h; // actual size on sprite sheet. This will inform the halfSize. Needs swapping if the sprite is rotated.
    while (success && elem)
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

      success = spriteName != nullptr &&
        elem->QueryIntAttribute(kTags[TAG_W], &w) == tinyxml2::XML_SUCCESS &&
        elem->QueryIntAttribute(kTags[TAG_H], &h) == tinyxml2::XML_SUCCESS &&
        elem->QueryIntAttribute(kTags[TAG_X], &x) == tinyxml2::XML_SUCCESS &&
        elem->QueryIntAttribute(kTags[TAG_Y], &y) == tinyxml2::XML_SUCCESS &&
        elem->QueryIntAttribute(kTags[TAG_ORIGINAL_W], &wOrig) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
        elem->QueryIntAttribute(kTags[TAG_ORIGINAL_H], &hOrig) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
        elem->QueryIntAttribute(kTags[TAG_OFFSET_X], &xOffs) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
        elem->QueryIntAttribute(kTags[TAG_OFFSET_Y], &yOffs) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE;
      LTRACEIF(!success, ("%s: definition of sprite %d missing a required element.",
        rawNameExt, numSprites));

      if (success)
      {
        auto spriteNameLen = strlen(spriteName);
        success = spriteNameLen <= buffer.capacity();
        XR_TRACEIF(TexturePath, !success,
          ("%s: sprite name '%s' too long (%z too many characters)", rawNameExt,
            spriteNameLen - buffer.capacity()));
      }

      if (success)
      {
        // Get name (of original image), strip extension.
        buffer = spriteName;
        char* pPeriod(buffer.rfind('.'));
        if (pPeriod)
        {
          *pPeriod = '\0';
          buffer.UpdateSize();
        }

        SpriteNameHashType hash = Hash::String32(buffer.c_str());
        auto iSprite = std::lower_bound(sprites.begin(), sprites.end(), hash, SpriteSheet::Entry::Compare);
        if (iSprite == sprites.end() || iSprite->mKey != hash)
        {
          iSprite = sprites.insert(iSprite, SpriteSheet::Entry{ hash, Sprite{} });
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
        Sprite& sprite = iSprite->mSprite;
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

        elem = elem->NextSiblingElement(kTags[TAG_SPRITE]);
        ++numSprites;
      }
    }

    if (success)
    {
      for (auto& s : sprites)
      {
        // Write sprite
        success = WriteBinaryStream(s, data);
        LTRACEIF(!success, ("%s: failed to write sprite %d.", rawNameExt,
          &s - sprites.data()));
      }
    }
  }

  return success;
}

} // nonamespace
#endif

//==============================================================================
bool SpriteSheet::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  ImagePathLenType imagePathLen;
  if (auto p = reader.ReadBytesWithSize(imagePathLen))
  {
    if (imagePathLen >= FilePath::kCapacity)
    {
      LTRACE(("%s: image path exceeds size limit.", m_debugPath.c_str()));
      return false;
    }
    else
    {
      mImagePath.assign(reinterpret_cast<char const*>(p), imagePathLen);
    }
  }

  NumSpritesType numSprites;
  if (!reader.Read(numSprites))
  {
    LTRACE(("%s: failed to read sprite count.", m_debugPath.c_str()));
    return false;
  }

  mSprites.resize(numSprites);

  auto i = mSprites.begin();
  auto iEnd = mSprites.end();
  while (i != iEnd)
  {
    if (!reader.Read(*i))
    {
      LTRACE(("%s: failed to read sprite %d.", m_debugPath.c_str(), i));
      return false;
    }
    ++i;
  }
  return true;
}

//==============================================================================
void SpriteSheet::OnUnload()
{
  mImagePath.clear();
  mSprites.clear();
}

//==============================================================================
FilePath const& SpriteSheet::GetImagePath() const
{
  return mImagePath;
}

//==============================================================================
Sprite const& SpriteSheet::Get(char const* name) const
{
  return Get(Hash::String32(name));
}

//==============================================================================
Sprite& SpriteSheet::Get(char const* name)
{
  return Get(Hash::String32(name));
}

//==============================================================================
Sprite const* SpriteSheet::TryGet(char const* name) const
{
  return TryGet(Hash::String32(name));
}

//==============================================================================
Sprite* SpriteSheet::TryGet(char const* name)
{
  return TryGet(Hash::String32(name));
}

}
