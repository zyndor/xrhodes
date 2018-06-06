//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/TexturePack.hpp"
#include "xr/utils.hpp"
#include "xr/FilePath.hpp"
#include "xr/BufferReader.hpp"
#include "xr/streamutils.hpp"
#include "xr/debug.hpp"
#include "tinyxml2.h"

#define LTRACE(format) XR_TRACE(TexturePack, format)
#define LTRACEIF(condition, format) XR_TRACEIF(TexturePack, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(TexturePack, "xtpk", 2, "xtp")

struct SpriteData
{
  SpriteVertexFormat  vertices[Quad::Vertex::kCount];
  Vector2 halfSize;
  Vector2 offset;

  void CalculatePositions(float baseWidth, float baseHeight)
  {
    float width = baseWidth;
    float height = baseHeight;
    bool rotated = Sprite::IsUVRotated(vertices);
    if(rotated)
    {
      width *= (vertices[Quad::Vertex::NE].uv0 - vertices[Quad::Vertex::SE].uv0).Magnitude();
      height *= (vertices[Quad::Vertex::SW].uv0 - vertices[Quad::Vertex::SE].uv0).Magnitude();
    }
    else
    {
      width *= (vertices[Quad::Vertex::SE].uv0 - vertices[Quad::Vertex::SW].uv0).Magnitude();
      height *= (vertices[Quad::Vertex::SW].uv0 - vertices[Quad::Vertex::NW].uv0).Magnitude();
    }

    width *= .5f;
    height *= .5f;
    halfSize.x = width;
    halfSize.y = height;

    if(rotated)
    {
      std::swap(width, height);
    }

    vertices[Quad::Vertex::NW].pos = Vector3(-width, height, .0f);
    vertices[Quad::Vertex::SW].pos = Vector3(-width, -height, .0f);
    vertices[Quad::Vertex::SE].pos = Vector3(width, -height, .0f);
    vertices[Quad::Vertex::NE].pos = Vector3(width, height, .0f);
  }

  void AddOffset(float x, float y)
  {
    offset.x += x;
    offset.y += y;

    Vector3 t(x, y, .0f);
    vertices[Quad::Vertex::NW].pos += t;
    vertices[Quad::Vertex::SW].pos += t;
    vertices[Quad::Vertex::SE].pos += t;
    vertices[Quad::Vertex::NE].pos += t;
  }
};

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
  bool success = xmlError == tinyxml2::XML_SUCCESS;
  LTRACEIF(!success, ("%s: failed to parse xml, code: %d", rawNameExt, xmlError));

  tinyxml2::XMLElement* elem = doc.RootElement();
  if (success)
  {
    success = elem != nullptr;
    LTRACEIF(!success, ("%s: no root element found.", rawNameExt));
  }

  // Base texture name
  const char* textureName = nullptr;
  if (success)
  {
    textureName = elem->Attribute(kTags[TAG_IMAGE_PATH]);
    success = textureName != nullptr;
    LTRACEIF(!success, ("%s: '%s' is a required attribute.", rawNameExt,
      kTags[TAG_IMAGE_PATH]));
  }

  if (success)
  {
    dependencies.push_back(textureName);

    Asset::HashType hash = Asset::Manager::HashPath(dependencies.back());
    success = WriteBinaryStream(hash, data);
    LTRACEIF(!success, ("%s: failed to write texture hash.", rawNameExt));
  }

  // Shader name
  const char* shaderName = nullptr;
  if (success)
  {
    shaderName = elem->Attribute(kTags[TAG_SHADER_PATH]);

    Asset::HashType hash = 0;
    if (shaderName != nullptr)
    {
      dependencies.push_back(shaderName);
      hash = Asset::Manager::HashPath(dependencies.back());
    }

    success = WriteBinaryStream(hash, data);
    LTRACEIF(!success, ("%s: failed to write shader hash.", rawNameExt));
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
    auto counter = elem->FirstChildElement("sprite");
    while (counter)
    {
      ++numSprites;
      counter = counter->NextSiblingElement("sprite");
    }

    success = WriteBinaryStream(numSprites, data);
    LTRACEIF(!success, ("%s: failed to write sprite count.", rawNameExt));
  }

  // Sprite data
  if (success)
  {
    elem = elem->FirstChildElement(kTags[TAG_SPRITE]);

    HardString<256> buffer;
    int x, y; // position of top left corner on sprite sheet
    int w, h; // actual size on sprite sheet. This will inform the halfSize. Needs swapping if the sprite is rotated.
    size_t numSprites = 0;
    while (success && elem != 0)
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

        // Calculate UVs - convert from bitmap-space to texture-space (bottom-left based).
        // TODO: consider support for non-openGL renderers where texture-space might be top-left based.
        auto omy = texHeight - y;
        AABB  uvs =
        {
          float(x) / texWidth,
          float(omy) / texHeight,
          float(x + w) / texWidth,
          float(omy - h) / texHeight,
        };

        // Get UV rotation attribute.
        const char* rotationAttrib = elem->Attribute(kTags[TAG_ROTATED]);
        const bool  isRotated = rotationAttrib != nullptr && strlen(rotationAttrib) > 0 &&
          rotationAttrib[0] == 'y';

        // If wOffs / hOffs isn't set, then use sprite width / height, depending on whether it's rotated.
        // We'll need these to correctly calculate offset (for padding).
        if (wOrig == 0)
        {
          wOrig = isRotated ? h : w;
        }

        if (hOrig == 0)
        {
          hOrig = isRotated ? w : h;
        }

        // Calculate UVs and vertex positions.
        SpriteData sprite;
        Sprite::CalculateUVs(uvs, isRotated, sprite.vertices);
        sprite.CalculatePositions(static_cast<float>(texWidth), static_cast<float>(texHeight));

        // Calculate offset for padding.
        // NOTE: y conversion (negation) applies since we're moving from bitmap space to model space.
        // TODO: consider support for non-openGL renderers where model-space might have negative Y.
        Vector2 offset;
        if (isRotated)
        {
          offset.x = xOffs - (wOrig - h) * .5f;
          offset.y = (hOrig - w) * .5f - yOffs;
        }
        else
        {
          offset.x = xOffs - (wOrig - w) * .5f;
          offset.y = (hOrig - h) * .5f - yOffs;
        }
        sprite.AddOffset(offset.x, offset.y);

        // Calculate halfSize.
        sprite.halfSize.x = wOrig * .5f;
        sprite.halfSize.y = hOrig * .5f;

        // Write sprite
        SpriteNameHashType hash = Hash::String32(buffer.c_str());
        success = WriteBinaryStream(hash, data) && WriteBinaryStream(sprite, data);
        LTRACEIF(!success, ("%s: failed to write sprite %d.", rawNameExt,
          numSprites));

        elem = elem->NextSiblingElement(kTags[TAG_SPRITE]);
        ++numSprites;
      }
    }
  }

  return success;
}

}
#endif

//==============================================================================
Sprite* TexturePack::Get(const char* name, bool allowMissing)
{
  XR_ASSERT(TexturePack, name != nullptr);
  Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr || allowMissing,
    ("%s: sprite '%s' doesn't exist.", m_debugPath.c_str(), name));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get(const char* name, bool allowMissing) const
{
  XR_ASSERT(TexturePack, name != nullptr);
  const Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr || allowMissing,
    ("%s: sprite '%s' doesn't exist.", m_debugPath.c_str(), name));
  return pSprite;
}

//==============================================================================
Sprite* TexturePack::Get(char const* name)
{
  XR_ASSERT(TexturePack, name != nullptr);
  Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr, ("%s: sprite '%s' doesn't exist.",
    m_debugPath.c_str(), name));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get(char const* name) const
{
  XR_ASSERT(TexturePack, name != nullptr);
  const Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr, ("%s: sprite '%s' doesn't exist.",
    m_debugPath.c_str(), name));
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
  bool success = reader.Read(textureHash);
  LTRACEIF(!success, ("%s: failed to read texture id.", m_debugPath.c_str()));

  FlagType flags = 0;
  if (success)
  {
    flags = GetFlags();
  }

  if (success)
  {
    m_texture = Manager::Find(Descriptor<Texture>(textureHash));
    success = m_texture != nullptr;
    LTRACEIF(!success, ("%s: failed to retrieve texture.", m_debugPath.c_str()));
  }

  NumSpritesType numSprites;
  if (success)
  {
    success = reader.Read(numSprites);
    LTRACEIF(!success, ("%s: failed to read sprite count.", m_debugPath.c_str()));
  }

  SpriteNameHashType spriteHash;
  SpriteData sprData;
  NumSpritesType i = 0;
  while (success && i < numSprites)
  {
    success = reader.Read(spriteHash) && reader.Read(sprData);
    if (success)
    {
      Sprite& spr = m_sprites[spriteHash];
      spr.Import(sprData.vertices);
      spr.SetHalfSize(sprData.halfSize.x, sprData.halfSize.y, false);
      ++i;
    }
    else
    {
      LTRACE(("%s: failed to read sprite %d.", m_debugPath.c_str(), i));
    }
  }
  return success;
}

//==============================================================================
void TexturePack::OnUnload()
{
  m_texture.Reset(nullptr);
  m_sprites.clear();
}

} // XR
