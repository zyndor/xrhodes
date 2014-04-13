#include <tinyxml.h>
#include "TexturePack.hpp"
#include "utils.hpp"

namespace XR
{

enum
{
  XML_IMAGE_PATH,
  XML_WIDTH,
  XML_HEIGHT,
  XML_SPRITE,
  XML_NAME,
  XML_X,
  XML_Y,
  XML_W,
  XML_H,
  XML_OFFSET_X,
  XML_OFFSET_Y,
  XML_OFFSET_W,
  XML_OFFSET_H,
  XML_ROTATED
};

static const char* karXml[] =
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

TexturePack::TexturePack()
: m_sprites(),
  m_pMaterial(0)
{}

TexturePack::~TexturePack()
{
  Clear();
}

bool TexturePack::Load(const char* pName, Material::GetCallback pGetCb,
  void* pGetCbData)
{
  XR_ASSERT(TexturePack, pName != 0);
  XR_ASSERT(TexturePack, strlen(pName) > 0);

  static const int kBufferSize = 256;

  // clean up
  m_sprites.clear();

  // load xml
  TiXmlDocument doc;
  bool  success(doc.LoadFile(pName));

  TiXmlElement* pElem(doc.RootElement());
  if(success)
  {
    success = pElem != 0;
  }

  const char*   pTextureName(0);
  if(success)
  {
    pTextureName = pElem->Attribute(karXml[XML_IMAGE_PATH]);

    success = pTextureName != 0;
    if(!success)
    {
      XR_TRACE(TexturePack, ("'%s' is a required attribute.",
        karXml[XML_IMAGE_PATH]));
    }
  }

  if(success)
  {
    XR_ASSERT(TexturePack, static_cast<int>(strlen(pTextureName)) <
      kBufferSize);

    char  arBuffer[kBufferSize];
    sprintf(arBuffer, pTextureName);

    char* pPeriod(strrchr(arBuffer, '.'));
    if(pPeriod != 0)
    {
      *pPeriod = '\0';
    }

    m_pMaterial = (*pGetCb)(arBuffer, pGetCbData);

    //else
    //{
    //  CIwTexture* pTexture(new CIwTexture);
    //  pTexture->LoadFromFile(pTextureName);
    //  pTexture->SetFiltering(true);
    //  pTexture->SetMipMapping(false);
    //  pTexture->Upload();

    //  m_pMaterial = new Material();
    //  m_pMaterial->SetTexture(pTexture);

    //  pMaterial = m_pMaterial;
    //}

    success = m_pMaterial != 0;
    if(!success)
    {
      XR_TRACE(TexturePack, ("Failed to find material '%s'.", arBuffer));
    }
  }

  int texWidth;
  int texHeight;
  if(success)
  {
    pElem->Attribute(karXml[XML_WIDTH], &texWidth);
    pElem->Attribute(karXml[XML_HEIGHT], &texHeight);

    if(texWidth != 0)
    {
      success = success && m_pMaterial->GetTexture(0).GetWidth() == texWidth;
      if(!success)
      {
        XR_TRACE(TexturePack, ("Texture width mismatch (%d vs %d)",
          m_pMaterial->GetTexture(0).GetWidth(), texWidth));  
      }
    }
    else
    {
      texWidth = m_pMaterial->GetTexture(0).GetWidth();
    }

    if(texHeight != 0)
    {
      success = success && m_pMaterial->GetTexture(0).GetHeight() == texHeight;
      if(!success)
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

  if(success)
  {
    pElem = pElem->FirstChildElement(karXml[XML_SPRITE]);

    char  arBuffer[kBufferSize];

    int x, y; // position of top left corner on sprite sheet
    int w, h; // size on sprite sheet
    int xOffs, yOffs; // amount of translation left and down
    int wOffs, hOffs; // size of finished sprite, including offset
    while(success && pElem != 0)
    {
      // create sprite
      const char* pSpriteName(pElem->Attribute(karXml[XML_NAME]));

      success = !(pSpriteName == 0 ||
        pElem->Attribute(karXml[XML_W], &w) == 0 ||
        pElem->Attribute(karXml[XML_H], &h) == 0 ||
        pElem->Attribute(karXml[XML_X], &x) == 0 ||
        pElem->Attribute(karXml[XML_Y], &y) == 0);
      
      if(success)
      {
        XR_ASSERT(TexturePack, static_cast<int>(strlen(pSpriteName)) < kBufferSize);

        // get name, strip extension
        sprintf(arBuffer, pSpriteName);

        char* pPeriod(strrchr(arBuffer, '.'));
        if(pPeriod != 0)
        {
          *pPeriod = '\0';
        }

        // set uvs
        AABB  uvs =
        {
          (float)(x) / texWidth,
          (float)(y) / texHeight,
          (float)(x + w) / texWidth,
          (float)(y + h) / texHeight,
        };

        // offset and total size
        if(pElem->Attribute(karXml[XML_OFFSET_X], &xOffs) == 0)
        {
          xOffs = 0;
        }

        if(pElem->Attribute(karXml[XML_OFFSET_Y], &yOffs) == 0)
        {
          yOffs = 0;
        }

        const char* pIsRotated(pElem->Attribute(karXml[XML_ROTATED]));
        bool  isRotated(pIsRotated != 0 && strlen(pIsRotated) > 0 &&
          pIsRotated[0] == 'y');

        if(pElem->Attribute(karXml[XML_OFFSET_W], &wOffs) == 0)
        {
          wOffs = isRotated ? h : w;
        }

        if(pElem->Attribute(karXml[XML_OFFSET_H], &hOffs) == 0)
        {
          hOffs = isRotated ? w : h;
        }

        Sprite  sprite;
        sprite.SetMaterial(m_pMaterial);

        if(isRotated)
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
        m_sprites.insert(Hash::String(arBuffer), sprite);
        
        pElem = pElem->NextSiblingElement(karXml[XML_SPRITE]);
      }
    }
  }

  return success;
}


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

Sprite* TexturePack::Get( const char* pName )
{
  XR_ASSERT(TexturePack, pName != 0);
  Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0, ("Sprite '%s' doesn't exist in TexturePack.", pName));
  return pSprite;
}

const Sprite* TexturePack::Get( const char* pName ) const
{
  XR_ASSERT(TexturePack, pName != 0);
  const Sprite* pSprite(Get(Hash::String(pName)));
  XR_ASSERTMSG(TexturePack, pSprite != 0, ("Sprite '%s' doesn't exist in TexturePack.", pName));
  return pSprite;
}


void  TexturePack::ScaleSprites(float x)
{
  ScaleSprites(x, x);
}

void  TexturePack::ScaleSprites(float x, float y)
{
  for(SpriteDictionary::iterator i0(m_sprites.begin()), i1(m_sprites.end()); i0 != i1; ++i0)
  {
    i0->value.Scale(x, y);
  }
}

void TexturePack::Clear()
{
  m_sprites.clear();
}

} // XR
