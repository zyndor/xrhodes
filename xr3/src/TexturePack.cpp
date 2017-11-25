//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/TexturePack.hpp"
#include "XR/utils.hpp"
#include "XR/FilePath.hpp"
#include "XR/debug.hpp"
#include "tinyxml.h"

namespace XR
{

//==============================================================================
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
  Tag_OFFSET_X,
  TAG_OFFSET_Y,
  TAG_OFFSET_W,
  TAG_OFFSET_H,
  TAG_ROTATED
};

static const char* const karTag[] =
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

//==============================================================================
TexturePack::TexturePack()
: m_sprites(),
  m_material()
{}

//==============================================================================
TexturePack::~TexturePack()
{
  Clear();
}

//==============================================================================
bool TexturePack::Load(char const* name, Asset::FlagType flags)
{
  XR_ASSERT(TexturePack, name != nullptr);
  XR_ASSERT(TexturePack, strlen(name) > 0);

  // clean up
  m_sprites.clear();

  // load xml
  TiXmlDocument doc;
  bool  success(doc.LoadFile(name));

  TiXmlElement* pElem(doc.RootElement());
  if (success)
  {
    success = pElem != nullptr;
  }

  const char* pTextureName = nullptr;
  if (success)
  {
    pTextureName = pElem->Attribute(karTag[TAG_IMAGE_PATH]);

    success = pTextureName != nullptr;
    if (!success)
    {
      XR_TRACE(TexturePack, ("'%s' is a required attribute.",
        karTag[TAG_IMAGE_PATH]));
    }
  }

  if (success)
  {
    FilePath texturePath(name);
    texturePath.Up();
    texturePath.UpdateSize(); // Workaround for a broken Up().
    texturePath /= pTextureName;
    if (auto ext = texturePath.GetExt())
    {
      *ext = '\0';
      texturePath.UpdateSize();
    }
    texturePath += ".mtl";

    m_material = Asset::Manager::Load<Material>(texturePath, flags |
      Asset::LoadSyncFlag | Asset::UnmanagedFlag);
    success = !CheckAnyMaskBits(m_material->GetFlags(), Asset::ErrorFlag);
    if (!success)
    {
      XR_TRACE(TexturePack, ("Failed to find material '%s'.", texturePath.c_str()));
    }
  }

  int texWidth;
  int texHeight;
  if (success)
  {
    pElem->Attribute(karTag[TAG_WIDTH], &texWidth);
    pElem->Attribute(karTag[TAG_HEIGHT], &texHeight);

    if (texWidth != 0)
    {
      success = success && m_material->GetTexture(0)->GetWidth() == texWidth;
      if (!success)
      {
        XR_TRACE(TexturePack, ("Texture width mismatch (%d vs %d)",
          m_material->GetTexture(0)->GetWidth(), texWidth));  
      }
    }
    else
    {
      texWidth = m_material->GetTexture(0)->GetWidth();
    }

    if (texHeight != 0)
    {
      success = success && m_material->GetTexture(0)->GetHeight() == texHeight;
      if (!success)
      {
        XR_TRACE(TexturePack, ("Texture height mismatch (%d vs %d)",
          m_material->GetTexture(0)->GetHeight(), texHeight));  
      }
    }
    else
    {
      texWidth = m_material->GetTexture(0)->GetHeight();
    }
  }

  if (success)
  {
    pElem = pElem->FirstChildElement(karTag[TAG_SPRITE]);

    HardString<256> buffer;
    int x, y; // position of top left corner on sprite sheet
    int w, h; // size on sprite sheet
    int xOffs, yOffs; // amount of translation left and down
    int wOffs, hOffs; // size of finished sprite, including offset
    while (success && pElem != nullptr)
    {
      // create sprite
      const char* pSpriteName(pElem->Attribute(karTag[TAG_NAME]));

      success = !(pSpriteName == nullptr ||
        pElem->Attribute(karTag[TAG_W], &w) == 0 ||
        pElem->Attribute(karTag[TAG_H], &h) == 0 ||
        pElem->Attribute(karTag[TAG_X], &x) == 0 ||
        pElem->Attribute(karTag[TAG_Y], &y) == 0);
      
      if (success)
      {
        // get name, strip extension
        buffer = pSpriteName;

        char* pPeriod(buffer.rfind('.'));
        if (pPeriod != nullptr)
        {
          *pPeriod = '\0';
        }

        // set uvs
        AABB  uvs =
        {
          float(x) / texWidth,
          float(y) / texHeight,
          float(x + w) / texWidth,
          float(y + h) / texHeight,
        };

        // offset and total size
        if (pElem->Attribute(karTag[Tag_OFFSET_X], &xOffs) == 0)
        {
          xOffs = 0;
        }

        if (pElem->Attribute(karTag[TAG_OFFSET_Y], &yOffs) == 0)
        {
          yOffs = 0;
        }

        const char* pIsRotated(pElem->Attribute(karTag[TAG_ROTATED]));
        bool  isRotated(pIsRotated != nullptr && strlen(pIsRotated) > 0 &&
          pIsRotated[0] == 'y');

        if (pElem->Attribute(karTag[TAG_OFFSET_W], &wOffs) == 0)
        {
          wOffs = isRotated ? h : w;
        }

        if (pElem->Attribute(karTag[TAG_OFFSET_H], &hOffs) == 0)
        {
          hOffs = isRotated ? w : h;
        }

        Sprite  sprite;
        sprite.SetMaterial(m_material);

        if (isRotated)
        {
          sprite.SetUVsRotatedProportional(uvs);
          sprite.OffsetVertices(xOffs - (wOffs - h) * .5f, yOffs - (hOffs - w) * .5f);
        }
        else
        {
          sprite.SetUVsProportional(uvs);
          sprite.OffsetVertices(xOffs - (wOffs - w) * .5f, yOffs - (hOffs - h) * .5f);
        }
        sprite.SetHalfSize(wOffs / 2, hOffs / 2, false);

        // add sprite
        m_sprites[Hash::String32(buffer.c_str())] = sprite;
        
        pElem = pElem->NextSiblingElement(karTag[TAG_SPRITE]);
      }
    }
  }

  return success;
}

//==============================================================================
Sprite* TexturePack::Get(const char* name, bool allowMissing)
{
  XR_ASSERT(TexturePack, name != nullptr);
  Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr || allowMissing,
    ("Sprite '%s' doesn't exist in TexturePack.", name));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get(const char* name, bool allowMissing) const
{
  XR_ASSERT(TexturePack, name != nullptr);
  const Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr || allowMissing,
    ("Sprite '%s' doesn't exist in TexturePack.", name));
  return pSprite;
}

//==============================================================================
Sprite* TexturePack::Get(char const* name)
{
  XR_ASSERT(TexturePack, name != nullptr);
  Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr, ("Sprite '%s' doesn't exist in TexturePack.", name));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get(char const* name) const
{
  XR_ASSERT(TexturePack, name != nullptr);
  const Sprite* pSprite(Get(Hash::String32(name)));
  XR_ASSERTMSG(TexturePack, pSprite != nullptr, ("Sprite '%s' doesn't exist in TexturePack.", name));
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

} // XR
