//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIRenderer.hpp"
#include "XR/IndexMesh.hpp"

namespace XR
{

//==============================================================================
UIRenderer::UIRenderer()
: m_hVertexFormat(Vertex::Register()),
  m_numSprites(0),
  m_vertices(),
  m_numSpritesRendered(0),
  m_numSpritesRenderable(0)
{}

//==============================================================================
UIRenderer::~UIRenderer()
{
  Shutdown();
}

//==============================================================================
void UIRenderer::Init(int numSprites)
{
  XR_ASSERT(UIRenderer, numSprites > 0);
  XR_ASSERTMSG(UIRenderer, m_materials.empty(), ("Already initialised. This is an error."));

  m_materials.resize(numSprites);

  int numVertices(numSprites * Sprite::kNumVertices);
  m_vertices.SetBuffer<Vertex>(numVertices);

  IndexMeshCore imc;
  imc.SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices, numSprites);
  m_ibo = Gfx::CreateIndexBuffer(imc.GetIndexBuffer(), 0);

  m_numSprites = numSprites;
}

//==============================================================================
UIRenderer::Vertex*  UIRenderer::NewSprite(Material::Ptr const& pMaterial)
{
  XR_ASSERTMSG(UIRenderer, m_numSprites > 0,
    ("UIRenderer must be initialized before calling NewSprite()."));
  XR_ASSERT(UIRenderer, m_numSpritesRenderable < m_numSprites);
  XR_ASSERT(UIRenderer, pMaterial != nullptr);

  int iSprite(m_numSpritesRenderable);
  ++m_numSpritesRenderable;

  m_materials[iSprite] = pMaterial;

  iSprite *= Sprite::kNumVertices;

  return m_vertices.Get<Vertex>() + iSprite;
}

//==============================================================================
void UIRenderer::Render()
{
  auto verts = m_vertices.Get<Vertex>() + m_numSpritesRendered * Sprite::kNumVertices;

  auto i = m_materials.begin() + m_numSpritesRendered;
  auto iEnd = i + (m_numSpritesRenderable - m_numSpritesRendered);
  while (i != iEnd)
  {
    auto iCompare = i + 1;
    while (iCompare != iEnd && *iCompare == *i)
    {
      ++iCompare;
    }

    const int numSprites = iCompare - i;
    const int numVertices = numSprites * Sprite::kNumVertices;
    const int numIndices = numSprites * Sprite::kNumIndices;

    auto vertsBytes = reinterpret_cast<uint8_t const*>(verts);
    Gfx::VertexBufferHandle vbo = Gfx::CreateVertexBuffer(m_hVertexFormat,
      { numVertices * sizeof(Vertex), vertsBytes }, Gfx::F_BUFFER_NONE);

    (*i)->Apply();
    Gfx::Draw(vbo, m_ibo, PrimType::TRI_LIST, 0, numIndices);
    Gfx::Destroy(vbo);

    verts += numVertices;
    i = iCompare;
  }

  m_numSpritesRendered = m_numSpritesRenderable;
}

//==============================================================================
void UIRenderer::ResetRendering()
{
  m_numSpritesRendered = 0;
}

//==============================================================================
void UIRenderer::Clear()
{
  m_numSpritesRenderable = 0;
  m_numSpritesRendered = 0;
}

//==============================================================================
void UIRenderer::Shutdown()
{
  Clear();

  std::vector<Material::Ptr>().swap(m_materials);

  Gfx::Destroy(m_ibo);
  m_ibo.Invalidate();

  m_vertices.ReleaseData();
  m_numSprites = 0;
}

} // XR
