//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/UIRenderer.hpp>

namespace XR
{

//==============================================================================
UIRenderer::UIRenderer()
: IndexMesh(),
  m_numSprites(0),
  m_parpMaterial(0),
  m_colors(),
  m_numSpritesRendered(0),
  m_numSpritesConsumed(0)
{}

//==============================================================================
UIRenderer::~UIRenderer()
{
  Shutdown();
}

//==============================================================================
void UIRenderer::Init( int numSprites )
{
  XR_ASSERT(UIRenderer, numSprites > 0);
  XR_ASSERTMSG(UIRenderer, m_parpMaterial == 0, ("Already initialised. This is an error."));

  m_parpMaterial = new Material*[numSprites];

  int numVertices(numSprites * Sprite::kNumVertices);
  InitStreams(numVertices);
  SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices, numSprites);
  m_colors.SetBuffer(sizeof(Color), numVertices);

  m_numSprites = numSprites;
}

//==============================================================================
FloatBuffer  UIRenderer::NewSprite( Material* pMaterial,
  const FloatBuffer& fbUV, Color color )
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;

  XR_ASSERT(UIRenderer, fbUV.GetNumElements() == Sprite::kNumVertices);
  m_uvs.Set(Sprite::kNumVertices, fbUV.Get<Vector2>(), iSprite);

  for (int i = 0; i < Sprite::kNumVertices; ++i)
  {
    m_colors.Set(iSprite + i, color);
  }

  return FloatBuffer::Adapt(m_vertices, iSprite, Sprite::kNumVertices);
}

//==============================================================================
FloatBuffer  UIRenderer::NewSprite( Material* pMaterial, Color color,
  FloatBuffer& fbUV)
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;
  fbUV = FloatBuffer::Adapt(m_uvs, iSprite, Sprite::kNumVertices);

  for (int i = 0; i < Sprite::kNumVertices; ++i)
  {
    m_colors.Set(iSprite + i, color);
  }

  return FloatBuffer::Adapt(m_vertices, iSprite, Sprite::kNumVertices);
}

//==============================================================================
FloatBuffer  UIRenderer::NewSprite( Material* pMaterial,
  FloatBuffer& fbUV, FloatBuffer& fbColor)
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;
  fbUV = FloatBuffer::Adapt(m_uvs, iSprite, Sprite::kNumVertices);
  fbColor = FloatBuffer::Adapt(m_uvs, iSprite, Sprite::kNumVertices);

  return FloatBuffer::Adapt(m_vertices, iSprite, Sprite::kNumVertices);
}

//==============================================================================
void UIRenderer::Render()
{
  int i(m_numSpritesRendered);
  FloatBuffer  fbUVs;
  FloatBuffer  fbCols;
  FloatBuffer  fbVerts;
  while (i < m_numSpritesConsumed)
  {
    Material*  pMaterial(m_parpMaterial[i]);

    int i1(i + 1);
    Material*  pMatCompare(m_parpMaterial[i1]);
    while (i1 < m_numSpritesConsumed && pMatCompare == pMaterial)
    {
      ++i1;
      ++pMatCompare;
    }

    int numSprites(i1 - i);
    int numVertices(numSprites * Sprite::kNumVertices);

    int iStart(i * Sprite::kNumVertices);
    
    fbUVs = FloatBuffer::Adapt(m_uvs, iStart, numVertices);
    fbCols = FloatBuffer::Adapt(m_colors, iStart, numVertices);
    fbVerts = FloatBuffer::Adapt(m_vertices, iStart, numVertices);

    Renderer::SetMaterial(pMaterial);
    Renderer::SetUVStream(fbUVs, 0);
    Renderer::SetColStream(fbCols);
    Renderer::SetVertStream(fbVerts);
    Renderer::DrawPrims(PrimType::TRI_LIST, &m_indices[0], numSprites *
      Sprite::kNumIndices);

    i = i1;
  }

  m_numSpritesRendered = m_numSpritesConsumed;
}

//==============================================================================
void UIRenderer::Clear()
{
  m_numSpritesConsumed = 0;
  m_numSpritesRendered = 0;
}

//==============================================================================
void UIRenderer::Shutdown()
{
  Clear();

  delete[] m_parpMaterial;
  m_parpMaterial = 0;

  m_vertices.ReleaseData();
  m_uvs.ReleaseData();
  m_colors.ReleaseData();
  IndexArray().swap(m_indices);

  m_numSprites = 0;
}

} // XR
