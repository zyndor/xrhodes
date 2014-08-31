#include "UIRenderer.hpp"

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
  m_colors.Create(RenderStream::F_COLOR, numVertices);

  m_numSprites = numSprites;
}

//==============================================================================
RenderStream  UIRenderer::NewSprite( Material* pMaterial,
  const RenderStream& rsUV, Color color )
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;
  m_uvs.Copy(rsUV, 0, rsUV.GetCapacity(), iSprite);

  for (int i = 0; i < Sprite::kNumVertices; ++i)
  {
    m_colors.Set(iSprite + i, color);
  }

  return m_vertices.SubStream(iSprite, Sprite::kNumVertices);
}

//==============================================================================
RenderStream  UIRenderer::NewSprite( Material* pMaterial, Color color,
  RenderStream& rsUV)
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;
  rsUV.Adapt(m_uvs, iSprite, Sprite::kNumVertices);

  for (int i = 0; i < Sprite::kNumVertices; ++i)
  {
    m_colors.Set(iSprite + i, color);
  }

  return m_vertices.SubStream(iSprite, Sprite::kNumVertices);
}

//==============================================================================
RenderStream  UIRenderer::NewSprite( Material* pMaterial,
  RenderStream& rsUV, RenderStream& rsColor)
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesConsumed < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != 0);

  int iSprite(m_numSpritesConsumed);
  ++m_numSpritesConsumed;

  m_parpMaterial[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;
  rsUV.Adapt(m_uvs, iSprite, Sprite::kNumVertices);
  rsColor.Adapt(m_uvs, iSprite, Sprite::kNumVertices);

  return m_vertices.SubStream(iSprite, Sprite::kNumVertices);
}

//==============================================================================
void UIRenderer::Render()
{
  int i(m_numSpritesRendered);
  RenderStream  rsUVs;
  RenderStream  rsCols;
  RenderStream  rsVerts;
  while (i < m_numSpritesConsumed)
  {
    Material*  pMaterial(m_parpMaterial[i]);

    int i1(i + 1);
    while (i1 < m_numSpritesConsumed && m_parpMaterial[i1] == pMaterial)
    {
      ++i1;
    }

    int numSprites(i1 - i);
    int numVertices(numSprites * Sprite::kNumVertices);

    int iStart(i * Sprite::kNumVertices);
    
    rsUVs.Adapt(m_uvs, iStart, numVertices);
    rsCols.Adapt(m_colors, iStart, numVertices);
    rsVerts.Adapt(m_vertices, iStart, numVertices);

    Renderer::SetMaterial(pMaterial);
    Renderer::SetUVStream(rsUVs, 0);
    Renderer::SetColStream(rsCols);
    Renderer::SetVertStream(rsVerts);
    Renderer::DrawPrims(PRIM_TRI_LIST, &m_indices[0], numSprites *
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

  m_vertices.Destroy();
  m_uvs.Destroy();
  m_colors.Destroy();
  m_indices.clear();

  m_numSprites = 0;
}

} // XR
