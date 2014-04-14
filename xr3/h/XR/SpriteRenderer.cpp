#include  <algorithm>
#include  "SpriteRenderer.hpp"

namespace XR
{

//==============================================================================
SpriteRenderer::SpriteRenderer()
: m_pPool(0),
  m_pRecords(0),
  m_mView(Matrix::s_identity),
  m_vForward(Vector3::s_zAxis),
  m_zNear(.0f),
  m_zFar(1000.0f),
  m_fadeColor(),
  m_useFadeColor(false)
{}

//==============================================================================
SpriteRenderer::~SpriteRenderer()
{
  m_pPool->Pop();
  m_pPool->Flush();
  delete m_pPool;
  m_pPool = 0;

  m_pRecords = 0;
}

//==============================================================================
void  SpriteRenderer::Init(int numRecords)
{
  XR_ASSERT(SpriteRenderer, m_pPool == 0);
  XR_ASSERT(SpriteRenderer, numRecords > 0);
  m_capacity = numRecords;

  int nodeSize(Align((sizeof(Record) + 2 * sizeof(void*)), 4));
  int memSize(Align(sizeof(RecordList), 4) + nodeSize * (2 * numRecords + 1));
  m_pPool = new XR::Pool(memSize);
  // Size of Record + 2 pointers assumed, 4 bytes aligned, otherwise,
  // based on compiler and libc++ implementation, the use of RecordList::_Node,
  // std::__detail::_List_node<Record> or other abomination would be required.

  // place list at the beginning of pool memory. this includes the initial
  // nodes created to indicate beginning/end of list.
  void* pMemList(m_pPool->Allocate(sizeof(RecordList)));
  m_pRecords = new (pMemList)
    RecordList(XR::SharedPoolAllocator<Record>(*m_pPool));

  // push frame - flushes shouldn't destroy the list.
  m_pPool->Push();
  
#if defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  InitStreams(numRecords * Sprite::kNumVertices);
  SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices,
    Sprite::kNumVertices, numRecords);
#else
  m_indices.resize(numRecords * Sprite::kNumIndices);
  
  int vertexOffset(0);
  int indexOffset(0);
  for(int i = 0; i < numRecords; ++i)
  {
    for(int j = 0; j < Sprite::kNumIndices; ++j)
    {
      m_indices[indexOffset + j] = Sprite::karIndices[j] + vertexOffset;
    }
    vertexOffset += Sprite::kNumVertices;
    indexOffset += Sprite::kNumIndices;
  }
#endif
}

//==============================================================================
void  SpriteRenderer::SetView(Matrix mView, float zNear, float zFar)
{
  XR_ASSERT(SpriteRenderer, zNear < zFar);
  
  m_vForward = Vector3(mView.zx, mView.zy, mView.zz);
  m_vForward.Normalise();

  //mView.Transpose();  // the actual view is a transpose, so this isn't needed.
  m_mView = mView;

  m_zNear = zNear;
  m_zFar = zFar;
  
  m_depthRangeRcp = zFar - zNear;
  if(m_depthRangeRcp > .0f)
  {
    m_depthRangeRcp = 1.0f / m_depthRangeRcp;
  }
}

//==============================================================================
void  SpriteRenderer::SetFadeColor(Color c)
{
  m_useFadeColor = true;
  m_fadeColor = c;
}

//==============================================================================
void  SpriteRenderer::Add(Color tint, const XR::Sprite* pSprite,
  const Vector3& position, float s, float rz)
{
  XR_ASSERT(SpriteRenderer, m_pRecords->size() < (size_t)m_capacity);
  Record  r =
  {
    Matrix(m_mView, position),
    tint,
    pSprite
  };

  if(s != 1.0f)
  {
    r.xForm.ScaleRot(s);
  }

  if(rz != .0f)
  {
    r.xForm.RotateZ(rz);
  }
  
  float zTemp = (r.xForm.t - m_mView.t).Dot(m_vForward);
  if(zTemp > m_zNear && zTemp < m_zFar) // if in front of camera
  {
    if(m_useFadeColor)
    {
      float distScale((zTemp - m_zNear) * m_depthRangeRcp);
      distScale *= distScale;
      
      r.tint *= (1.0f - distScale);
      r.tint += m_fadeColor * distScale;
    }
    
    r.zTemp = zTemp;
    RecordList::iterator iInsert(std::lower_bound(m_pRecords->begin(),
      m_pRecords->end(), r, Record::Compare));

    m_pRecords->insert(iInsert, r);
  }
}

//==============================================================================
void  SpriteRenderer::Add(Color tint, const XR::Sprite* pSprite,
  const Vector3& position, float sx, float sy, float rz)
{
  XR_ASSERT(SpriteRenderer, m_pRecords->size() < (size_t)m_capacity);
  Record  r =
  {
    Matrix(m_mView, position),
    tint,
    pSprite
  };

  if(sx != 1.0f)
  {
    r.xForm.ScaleX(sx);
  }

  if(sy != 1.0f)
  {
    r.xForm.ScaleY(sx);
  }

  if(rz != .0f)
  {
    r.xForm.RotateZ(rz);
  }

  float zTemp = (r.xForm.t - m_mView.t).Dot(m_vForward);
  if(zTemp > m_zNear && zTemp < m_zFar) // if in front of camera
  {
    if(m_useFadeColor)
    {
      float distScale((zTemp - m_zNear) * m_depthRangeRcp);
      distScale *= distScale;
      
      r.tint *= (1.0f - distScale);
      r.tint += m_fadeColor * distScale;
    }
    
    r.zTemp = zTemp;
    RecordList::iterator iInsert(std::lower_bound(m_pRecords->begin(),
      m_pRecords->end(), r, Record::Compare));

    m_pRecords->insert(iInsert, r);
  }
}

//==============================================================================
void  SpriteRenderer::Render()
{
  // allocate colour / uv / vertex streams
  int numVerts(m_pRecords->size());
  if(numVerts <= 0)
  {
    return;
  }
  numVerts *= XR::Sprite::kNumVertices;
  
  RenderStream* pCols(Renderer::AllocStream(RenderStream::F_COLOR, numVerts));
  XR_ASSERT(SpriteRenderer, pCols != 0);
#if defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  RenderStream* pUVs(&m_uvs);
  RenderStream* pVerts(&m_vertices);
#else
  RenderStream* pUVs(Renderer::AllocStream(RenderStream::F_VECTOR2, numVerts));
  XR_ASSERT(SpriteRenderer, pUVs != 0);
  RenderStream* pVerts(Renderer::AllocStream(RenderStream::F_VECTOR3, numVerts));
  XR_ASSERT(SpriteRenderer, pVerts != 0);
#endif
  
  int           vertexOffset(0);
  int           indexOffset(0);

  RenderStream  colBatch;
  RenderStream  uvBatch;
  RenderStream  vertBatch;

  for(RecordList::const_iterator i0(m_pRecords->begin()),
    i1(m_pRecords->end()); i0 != i1;)
  {
    Material*  pMaterial(i0->pSprite->GetMaterial());

    // check for consecutive sprites with the same material
    int numSprites(1);

    RecordList::const_iterator j0(i0);
    while(++j0 != i1)
    {
      if(j0->pSprite->GetMaterial() != pMaterial)
      {
        break;
      }
      ++numSprites;
    }

    // calculate number of vertices
    numVerts = numSprites * XR::Sprite::kNumVertices;

    colBatch.Adapt(*pCols, vertexOffset, numVerts);
    uvBatch.Adapt(*pUVs, vertexOffset, numVerts);
    vertBatch.Adapt(*pVerts, vertexOffset, numVerts);

    // set the material
    Renderer::SetMaterial(i0->pSprite->GetMaterial());

    // render sprites
    while(i0 != j0)
    {
      pCols->Set(vertexOffset, i0->tint);
      pCols->Set(vertexOffset + 1, i0->tint);
      pCols->Set(vertexOffset + 2, i0->tint);
      pCols->Set(vertexOffset + 3, i0->tint);

      i0->pSprite->CopyUVsTo(vertexOffset, *pUVs);

      for(int i = 0; i < XR::Sprite::kNumVertices; ++i)
      {
        pVerts->Set(vertexOffset + i,
          i0->xForm.TransformVec(i0->pSprite->GetVertices().GetVector3(i)));
      }

      vertexOffset += XR::Sprite::kNumVertices;
      ++i0;
    }

    // indices
    int numIndices(numSprites * Sprite::kNumIndices);
    
    // render
    Renderer::SetColStream(colBatch);
    Renderer::SetUVStream(uvBatch, 0);
    Renderer::SetVertStream(vertBatch);
    Renderer::DrawPrims(PRIM_TRI_LIST, &m_indices[0] + indexOffset,
      numIndices);
    
    indexOffset += numIndices;
  }
}

//==============================================================================
void  SpriteRenderer::Clear()
{
  XR_ASSERT(SpriteRenderer, m_pRecords != 0);
  m_pRecords->clear();
  m_pPool->Flush();
  m_useFadeColor = false;
}

} // XR
