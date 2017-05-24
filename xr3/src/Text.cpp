//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Text.hpp>

namespace XR {

//==============================================================================
Text::Text()
: IndexMesh(),
  m_pFont(0),
  m_hAlign(HA_CENTER),
  m_vAlign(VA_MIDDLE),
  m_boxWidth(.0f),
  m_boxHeight(.0f),
  m_text(),
  m_lines(),
  m_height(.0f)
{}

//==============================================================================
Text::~Text()
{}

//==============================================================================
void Text::SetFont(const Font& f, bool update)
{
  m_pFont = &f;
  m_pMaterial = f.GetMaterial();
  if (update)
  {
    Update();
  }
}

//==============================================================================
void Text::SetHorizontalAlignment(HAlign ha, bool update)
{
  m_hAlign = ha;
  if (update)
  {
    Update();
  }
}

//==============================================================================
void Text::SetVerticalAlignment(VAlign va, bool update)
{
  m_vAlign = va;
  if (update)
  {
    Update();
  }
}

//==============================================================================
void Text::SetText(const char* pText, bool update)
{
  XR_ASSERT(Text, pText != 0);
  m_text = pText;
  if (update)
  {
    Update();
  }
}

//==============================================================================
void  Text::SetSize(float boxWidth, float boxHeight, bool update)
{
  XR_ASSERT(Text, boxWidth >= .0f);
  XR_ASSERT(Text, boxHeight >= .0f);
  
  m_boxWidth = boxWidth;
  m_boxHeight = boxHeight;
  if (update)
  {
    Update();
  }
}

//==============================================================================
void  Text::Update()
{
  m_lines.clear();
  
  // prepare lines
  XR_ASSERT(Text, m_pFont != 0);
  const float kFontSize(m_pFont->GetSize());
  const float kFontSizeWithBase(kFontSize + m_pFont->GetBase());
  
  float       vFill(.0f); // vertical fill
  int         numGlyphsTotal(0);
  LineList    lines;
  const char* pText(m_text.c_str());
  const char* pTextEnd(pText + m_text.size());
  while (pText != pTextEnd && vFill + kFontSizeWithBase <= m_boxHeight)
  {
    // look for first non-wspace
    while (pText != pTextEnd && iswspace(*pText))
    {
      ++pText;
    }
    
    int c(*pText);
    
    const Font::Glyph*  pGlyph(m_pFont->GetGlyph(c));
    if (pGlyph != 0 && pGlyph->xAdvance > m_boxWidth)
    {
      // horizontal space is less than width of first glyph; abort
      break;
    }
    
    vFill += kFontSize;
    
    int   i(0);
    float hFill(.0f);
    float hFillFirstSpace(.0f);
    int   numGlyphsTotalFirstSpace(0);
    int   space0(-1);
    int   space1(-1);
    bool  isInSpace(false);
    
    while (c != '\n')
    {
      if (iswspace(c))
      {
        if (!isInSpace)
        {
          space0 = i;
          isInSpace = true;
          hFillFirstSpace = hFill;
          numGlyphsTotalFirstSpace = numGlyphsTotal;
        }
      }
      else
      {
        ++numGlyphsTotal;
        if (isInSpace)
        {
          space1 = i;
          isInSpace = false;
        }
      }
      
      if (pGlyph != 0)
      {
        hFill += pGlyph->xAdvance;
        if (hFill > m_boxWidth)
        {
          break;
        }
      }
      
      ++i;
      c = pText[i];
      if (c == 0)
      {
        break;
      }
      
      pGlyph = m_pFont->GetGlyph(c);
    }

    bool  spaceCut(hFill > m_boxWidth && space0 >= 0);
    if (spaceCut)
    {
      i = space0;
      hFill = hFillFirstSpace;
      numGlyphsTotal = numGlyphsTotalFirstSpace;
    }
    
    Line  l;
    l.text.assign(pText, i);
    l.width = ceilf(hFill);
    
    lines.push_back(l);
    
    pText += i;
  }
  
  // transfer lines
  m_lines.reserve(lines.size());
  for (LineList::const_iterator i0(lines.begin()), i1(lines.end()); i0 != i1;
    ++i0)
  {
    m_lines.push_back(i0->width);
  }
  
  //m_lines.insert(m_lines.end(), lines.begin(), lines.end());
  m_height = ceilf(m_pFont->CalcHeight((int)m_lines.size()));
  
  // generate uvs and coordinates
  InitStreams(numGlyphsTotal * Sprite::kNumVertices);
  SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices, 
    Sprite::kNumVertices, numGlyphsTotal);

  int vertOffset(0);
  int indexOffset(0);

  float vAlignAmount(.0f);
  switch (m_vAlign)
  {
  case VA_TOP:
    vAlignAmount = m_boxHeight * -.5f;
    break;
    
  case VA_MIDDLE:
    vAlignAmount = m_height * -.5f;
    break;

  case VA_BOTTOM:
    vAlignAmount = m_boxHeight * .5f - m_height;
    break;
  }

  float fillVer(vAlignAmount);
  for (LineList::const_iterator i0(lines.begin()), i1(lines.end());
    i0 != i1; ++i0)
  {
    const std::string&  kTextString(i0->text);
    float hAlignAmount(.0f);
    switch (m_hAlign)
    {
    case  HA_LEFT:
      hAlignAmount = m_boxWidth * -.5f;
      break;
      
    case HA_CENTER:
      hAlignAmount = i0->width * -.5f;
      break;

    case HA_RIGHT:
      hAlignAmount = m_boxWidth * .5f - i0->width;
      break;
    }

    float fillHor(hAlignAmount);

    // copy uvs, calculate vertices
    const Font::Glyph*  pGlyph;
    for (std::string::const_iterator j0(kTextString.begin()),
      j1(kTextString.end()); j0 != j1; ++j0)
    {
      int c(*j0);

      pGlyph = m_pFont->GetGlyph(c);
      if (pGlyph != 0)
      {
        if (!iswspace(c))
        {
          // copy uvs
          const AABB& uvs(pGlyph->uvs);
          m_uvs.Set(Sprite::VI_NW + vertOffset, Vector2(uvs.left, uvs.top));
          m_uvs.Set(Sprite::VI_SW + vertOffset, Vector2(uvs.left, uvs.bottom));
          m_uvs.Set(Sprite::VI_SE + vertOffset, Vector2(uvs.right, uvs.bottom));
          m_uvs.Set(Sprite::VI_NE + vertOffset, Vector2(uvs.right, uvs.top));
        
          // calculate vertices
          float x(fillHor + pGlyph->xOffs);
          float y(fillVer + pGlyph->yOffs);
          float x1(x + pGlyph->w);
          float y1(y + pGlyph->h);

          m_vertices.Set(Sprite::VI_NW + vertOffset, Vector3(x, y, .0f));
          m_vertices.Set(Sprite::VI_SW + vertOffset, Vector3(x, y1, .0f));
          m_vertices.Set(Sprite::VI_SE + vertOffset, Vector3(x1, y1, .0f));
          m_vertices.Set(Sprite::VI_NE + vertOffset, Vector3(x1, y, .0f));
          
          vertOffset += Sprite::kNumVertices;
          indexOffset += Sprite::kNumIndices;
        }

        fillHor += pGlyph->xAdvance;
      }
    }
    
    fillVer += kFontSize;
  }
}

//==============================================================================
float Text::GetMaxLineWidth() const
{
  float   max(.0f);
  for (FloatArray::const_iterator i0(m_lines.begin()), i1(m_lines.end());
    i0 != i1; ++i0)
  {
    float temp(*i0);
    if (temp > max)
    {
      max = temp;
    }
  }
  return max;
}

//==============================================================================
FloatBuffer*  Text::CreateColStream(const Color& c) const
{
  const int n(GetVertices().GetNumElements());
  FloatBuffer* pCols(Renderer::AllocBuffer(sizeof(Color), n));
  for(int i = 0; i < n; ++i)
  {
    pCols->Set(i, c);
  }

  return pCols;
}

} // XR
