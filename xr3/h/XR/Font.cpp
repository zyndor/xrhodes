#include <limits>
#include <iostream>
#include "File.hpp"
#include "Font.hpp"
#include "utils.hpp"

namespace XR
{

//==============================================================================
enum
{
  ATT_SIZE,
  ATT_OUTLINE,
  ATT_BASE,
  ATT_SCALE_W,
  ATT_SCALE_H,
  ATT_FILE,
  ATT_CHARS_COUNT,
  ATT_CHAR_ID,
  ATT_X,
  ATT_Y,
  ATT_WIDTH,
  ATT_HEIGHT,
  ATT_XOFFSET,
  ATT_YOFFSET,
  ATT_XADVANCE,
  kNumAttribs  
};

//==============================================================================
static const char*  karAttribs[]=
{
  "size=",
  "outline=",
  "base=",
  "scaleW=",
  "scaleH=",
  "file=",
  "chars count=",
  "char id=",
  "x=",
  "y=",
  "width=",
  "height=",
  "xoffset=",
  "yoffset=",
  "xadvance="
};

//==============================================================================
const char* FindValue(const char* pBuffer, const char* pKey, const char*& pResult)
{
  XR_ASSERT(FindValue, pBuffer != 0);
  XR_ASSERT(FindValue, pKey != 0);
  const char* pNext = strstr(pBuffer, pKey);
  if(pNext != 0)
  {
    int keySize(strlen(pKey));
    pResult = pNext + keySize;

    pNext = strpbrk(pResult, " \n\t\0");
  }
  else
  {
    pResult = 0;  
  }
  return pNext;
}

//==============================================================================
Font::Font()
: m_size(.0f),
  m_base(.0f),
  //m_arGlyphs(),
  m_dGlyphs(),
  m_pMaterial(0)
{}

//==============================================================================
Font::~Font()
{}

//==============================================================================
bool  Font::Load( const char* pName, Material::GetCallback pGetMaterialCb,
        void* pGetMaterialCbData )
{
  XR_ASSERT(Font, pName != 0);
  XR_ASSERT(Font, strlen(pName) > 0);

  Clear();

  XR_ASSERT(Font, pName != 0);
  int hFile(File::Open(pName, "rb"));
  if(hFile == File::INVALID_HANDLE)
  {
    return false;
  }

  int64 size(File::GetSize(hFile));
  XR_ASSERT(Font, size >= 0);
  char* pBuffer(new char[size]);

  bool success(File::Read(size, 1, hFile, pBuffer) != 0);

  File::Close(hFile);
  if(success)
  {
    const char* pRead(pBuffer);
    const char* pTempRead;
    const char* pValue;

    pTempRead = FindValue(pRead, karAttribs[ATT_SIZE], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      m_size = (float)atoi(pValue);
      pRead = pTempRead;
    }

    float fOutline(.0f);
    pTempRead = FindValue(pRead, karAttribs[ATT_OUTLINE], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      fOutline = (float)atoi(pValue);
      m_size += fOutline * 2.0f;
      pRead = pTempRead;
    }
    
    pTempRead = FindValue(pRead, karAttribs[ATT_BASE], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      m_base = m_size - atoi(pValue);
      pRead = pTempRead;
    }

    float fTexWidth(.0f);
    pTempRead = FindValue(pRead, karAttribs[ATT_SCALE_W], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      fTexWidth = (float)atoi(pValue);
      if(fTexWidth > .0f)
      {
        fTexWidth = 1.0f / fTexWidth;
      }
      pRead = pTempRead;
    }

    float fTexHeight(.0f);
    pTempRead = FindValue(pRead, karAttribs[ATT_SCALE_H], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      fTexHeight = (float)atoi(pValue);
      if(fTexHeight > .0f)
      {
        fTexHeight = 1.0f / fTexHeight;
      }
      pRead = pTempRead;
    }

    pTempRead = FindValue(pRead, karAttribs[ATT_FILE], pValue);
    success = success && (pValue != 0);
    if(success)
    {
      if(pValue[0] == '"')
      {
        ++pValue;
      }

      const char* pClosingQuote(strchr(pValue, '\"'));
      const char* pPeriod(strchr(pValue, '.'));

      const int kBufferSize = 256;
      char  arBuffer[kBufferSize];

      success = !(pClosingQuote == 0 || pPeriod == 0) &&
        pPeriod < pClosingQuote &&
        (pPeriod - pValue) < kBufferSize;
      if(success)
      {
        int textureNameSize(pPeriod - pValue);
        XR_ASSERT(Font, textureNameSize + 1 < kBufferSize);  // 0-terminated
        strncpy(arBuffer, pValue, textureNameSize);
        arBuffer[textureNameSize] = '\0';

        m_pMaterial = (*pGetMaterialCb)(arBuffer, pGetMaterialCbData);
      }
    }

    success = success && m_pMaterial != 0;
    if(!success)
    {
      XR_TRACE(Font, ("The value of the file attribute needs to be the name of a valid material in pGroup (the extension is ignored)."));
    }

    if(success)
    {
      pTempRead = FindValue(pRead, karAttribs[ATT_CHARS_COUNT], pValue);
    }
    
    success = success && pValue != 0;
    if(!success)
    {
      XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_CHARS_COUNT]));
    }

    int numChars(0);
    success = success && sscanf(pValue, "%d", &numChars) > 0;
    if(success)
    {
      pRead = pTempRead;
    }

    int     chr;
    Glyph*  pGlyph;
    while(success && numChars > 0)
    {
      pTempRead = FindValue(pRead, karAttribs[ATT_CHAR_ID], pValue);

      success = pValue != 0;
      if(success)
      {
        chr = atoi(pValue);
        if(chr >= 0 && chr < kNumBasicGlyphs)
        {
          pGlyph = m_arGlyphs + chr;
        }
        else
        {
          Glyph&  g(m_dGlyphs[chr]);
          pGlyph = &g;
        }
        
        float x;
        pTempRead = FindValue(pTempRead, karAttribs[ATT_X], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_X]));
          break;
        }
        x = (float)(atoi(pValue));

        float y;
        pTempRead = FindValue(pTempRead, karAttribs[ATT_Y], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_Y]));
          break;
        }
        y = (float)(atoi(pValue));

        pTempRead = FindValue(pTempRead, karAttribs[ATT_WIDTH], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_WIDTH]));
          break;
        }
        pGlyph->w = (float)atoi(pValue);

        pTempRead = FindValue(pTempRead, karAttribs[ATT_HEIGHT], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_HEIGHT]));
          break;
        }
        pGlyph->h = (float)atoi(pValue);

        pTempRead = FindValue(pTempRead, karAttribs[ATT_XOFFSET], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_XOFFSET]));
          break;
        }
        pGlyph->xOffs = (float)atoi(pValue) + fOutline;

        pTempRead = FindValue(pTempRead, karAttribs[ATT_YOFFSET], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_YOFFSET]));
          break;
        }
        pGlyph->yOffs = (float)atoi(pValue) + fOutline;

        pTempRead = FindValue(pTempRead, karAttribs[ATT_XADVANCE], pValue);
        success = pValue != 0;
        if(!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karAttribs[ATT_XADVANCE]));
          break;
        }
        pGlyph->xAdvance = (float)atoi(pValue) + fOutline * 2.0f;

        // uvs
        pGlyph->uvs.left = x * fTexWidth;
        pGlyph->uvs.right = (x + pGlyph->w) * fTexWidth;
        pGlyph->uvs.top = y * fTexHeight;
        pGlyph->uvs.bottom = (y + pGlyph->h) * fTexHeight;

        pRead = pTempRead;
      }
      --numChars;
    }
  }

  delete[] pBuffer;

  return success;
}

//==============================================================================
void  Font::Clear()
{
  m_size = .0f;
  m_base = .0f;
  
  memset(m_arGlyphs, 0x00, sizeof(m_arGlyphs));
  m_dGlyphs.clear();
}
  
} // XR
