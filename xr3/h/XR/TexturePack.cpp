#include <tinyxml.h>
#include "TexturePack.hpp"
#include "utils.hpp"
#include "strings.hpp"

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
  m_pMaterial(0)
{}

//==============================================================================
TexturePack::~TexturePack()
{
  Clear();
}

//==============================================================================
bool TexturePack::Load(const char* pName, Material::GetCallback pGetCb,
  void* pGetCbData)
{
  XR_ASSERT(TexturePack, pName != 0);
  XR_ASSERT(TexturePack, strlen(pName) > 0);

  // clean up
  m_sprites.clear();

  // load xml
  TiXmlDocument doc;
  bool  success(doc.LoadFile(pName));

  TiXmlElement* pElem(doc.RootElement());
  if (success)
  {
    success = pElem != 0;
  }

  const char*   pTextureName(0);
  if (success)
  {
    pTextureName = pElem->Attribute(karTag[TAG_IMAGE_PATH]);

    success = pTextureName != 0;
    if (!success)
    {
      XR_TRACE(TexturePack, ("'%s' is a required attribute.",
        karTag[TAG_IMAGE_PATH]));
    }
  }

  if (success)
  {
    LString buffer(pTextureName);
    char* pPeriod(buffer.rfind('.'));
    if (pPeriod != 0)
    {
      *pPeriod = '\0';
    }

    m_pMaterial = (*pGetCb)(buffer.c_str(), pGetCbData);

    success = m_pMaterial != 0;
    if (!success)
    {
      XR_TRACE(TexturePack, ("Failed to find material '%s'.", buffer));
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
      success = success && m_pMaterial->GetTexture(0).GetWidth() == texWidth;
      if (!success)
      {
        XR_TRACE(TexturePack, ("Texture width mismatch (%d vs %d)",
          m_pMaterial->GetTexture(0).GetWidth(), texWidth));  
      }
    }
    else
    {
      texWidth = m_pMaterial->GetTexture(0).GetWidth();
    }

    if (texHeight != 0)
    {
      success = success && m_pMaterial->GetTexture(0).GetHeight() == texHeight;
      if (!success)
      {
        XR_TRACE(TexturePack, ("Texture height mismatch (%d vs %d)",
          m_pMaterial->GetTexture(0).GetHeight(), texHeight));  
      }
    }
    else
    {
      texWidth = m_pMaterial->GetTexture(0).GetHeight();
    }
  }

  if (success)
  {
    pElem = pElem->FirstChildElement(karTag[TAG_SPRITE]);

    LString buffer;
    int x, y; // position of top left corner on sprite sheet
    int w, h; // size on sprite sheet
    int xOffs, yOffs; // amount of translation left and down
    int wOffs, hOffs; // size of finished sprite, including offset
    while (success && pElem != 0)
    {
      // create sprite
      const char* pSpriteName(pElem->Attribute(karTag[TAG_NAME]));

      success = !(pSpriteName == 0 ||
        pElem->Attribute(karTag[TAG_W], &w) == 0 ||
        pElem->Attribute(karTag[TAG_H], &h) == 0 ||
        pElem->Attribute(karTag[TAG_X], &x) == 0 ||
        pElem->Attribute(karTag[TAG_Y], &y) == 0);
      
      if (success)
      {
        // get name, strip extension
        buffer = pSpriteName;

        char* pPeriod(buffer.rfind('.'));
        if (pPeriod != 0)
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
        bool  isRotated(pIsRotated != 0 && strlen(pIsRotated) > 0 &&
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
        sprite.SetMaterial(m_pMaterial);

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
        m_sprites[Hash::String(buffer.c_str())] = sprite;
        
        pElem = pElem->NextSiblingElement(karTag[TAG_SPRITE]);
      }
    }
  }

  return success;
}

//==============================================================================
Sprite* TexturePack::Get( const char* pName, bool allowMissing )
{
  XR_ASSERT(TexturePack, pName != 0);
  Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0 || allowMissing,
    ("Sprite '%s' doesn't exist in TexturePack.", pName));
  return pSprite;
}

const Sprite* TexturePack::Get( const char* pName, bool allowMissing ) const
{
  XR_ASSERT(TexturePack, pName != 0);
  const Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0 || allowMissing,
    ("Sprite '%s' doesn't exist in TexturePack.", pName));
  return pSprite;
}

//==============================================================================
Sprite* TexturePack::Get( const char* pName )
{
  XR_ASSERT(TexturePack, pName != 0);
  Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0, ("Sprite '%s' doesn't exist in TexturePack.", pName));
  return pSprite;
}

//==============================================================================
const Sprite* TexturePack::Get( const char* pName ) const
{
  XR_ASSERT(TexturePack, pName != 0);
  const Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0, ("Sprite '%s' doesn't exist in TexturePack.", pName));
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
