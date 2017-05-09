//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <XR/SpriteRenderer.hpp>

namespace XR
{

//==============================================================================
SpriteRenderer::SpriteRenderer()
: m_pool(),
  m_pRecords(0)/*,
  m_mView(Matrix::s_identity),
  m_vForward(Vector3::s_zAxis),
  m_zNear(.0f),
  m_zFar(1000.0f)*/
{}

//==============================================================================
SpriteRenderer::~SpriteRenderer()
{
  m_pool.Pop();
  m_pool.Flush();

  m_pRecords = 0;
}

//==============================================================================
void  SpriteRenderer::Init(int numRecords)
{
  XR_ASSERT(SpriteRenderer, numRecords > 0);
  m_capacity = numRecords;

  // pop all frames from pool (really should be just 1)
  while(m_pool.GetNumFrames() > 0)
  {
    m_pool.Pop();
  }
  m_pool.Flush();

  // Size of Record + 2 pointers assumed, 4 bytes aligned, otherwise,
  // based on compiler and libc++ implementation, the use of RecordList::_Node,
  // std::__detail::_List_node<Record> or other abomination would be required.
  int nodeSize(Align((sizeof(Record) + 2 * sizeof(void*)), 4));
  int memSize(Align(sizeof(RecordList), 4) + nodeSize * (2 * numRecords + 1));
  m_pool.SetBuffer(memSize, true, 0);

  // place list at the beginning of pool memory. this includes the initial
  // nodes created to indicate beginning/end of list.
  void* pMemList(m_pool.Allocate(sizeof(RecordList)));
  m_pRecords = new (pMemList)
    RecordList(XR::SharedPoolAllocator<Record>(m_pool));

  // push frame - flushes must not destroy the list.
  m_pool.Push();
  
#if defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  InitStreams(numRecords * Sprite::kNumVertices);
  SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices,
    Sprite::kNumVertices, numRecords);
#else
  m_indices.resize(numRecords * Sprite::kNumIndices);
  
  int vertexOffset(0);
  int indexOffset(0);
  for (int i = 0; i < numRecords; ++i)
  {
    for (int j = 0; j < Sprite::kNumIndices; ++j)
    {
      m_indices[indexOffset + j] = Sprite::karIndices[j] + vertexOffset;
    }
    vertexOffset += Sprite::kNumVertices;
    indexOffset += Sprite::kNumIndices;
  }
#endif
}

//==============================================================================
void  SpriteRenderer::SetView(Matrix mView, float zNear/*, float zFar*/)
{
  //XR_ASSERT(SpriteRenderer, zNear < zFar);
  XR_ASSERT(SpriteRenderer, zNear > .0f);
  
  m_vForward = Vector3(mView.zx, mView.zy, mView.zz).Normalise();
  
  //mView.Transpose();  // the actual view is a transpose, so this isn't needed.
  m_mView = mView;

  m_zNear = zNear;
  //m_zFar = zFar;
}

//==============================================================================
void  SpriteRenderer::Add(Color tint, const XR::Sprite* pSprite,
  const Vector3& position, float s, float rz, bool billboard)
{
  XR_ASSERT(SpriteRenderer, m_pRecords->size() < (size_t)m_capacity);
  Record  r =
  {
    billboard ? Matrix(m_mView, position) : Matrix(position),
    tint,
    pSprite
  };

  if (s != 1.0f)
  {
    r.xForm.ScaleRot(s);
  }

  if (rz != .0f)
  {
    r.xForm.RotateZ(rz);
  }
  
  float zTemp = (r.xForm.t - m_mView.t).Dot(m_vForward);
  if (zTemp > m_zNear/* && zTemp < m_zFar*/) // if in front of camera
  {
    r.zTemp = zTemp;
    RecordList::iterator iInsert(std::lower_bound(m_pRecords->begin(),
      m_pRecords->end(), r, Record::Compare));
    m_pRecords->insert(iInsert, r);
  }
}

//==============================================================================
void  SpriteRenderer::Add(Color tint, const XR::Sprite* pSprite,
  const Vector3& position, float sx, float sy, float rz, bool billboard)
{
  XR_ASSERT(SpriteRenderer, m_pRecords->size() < (size_t)m_capacity);
  Record  r =
  {
    billboard ? Matrix(m_mView, position) : Matrix(position),
    tint,
    pSprite
  };

  if (sx != 1.0f)
  {
    r.xForm.ScaleX(sx);
  }

  if (sy != 1.0f)
  {
    r.xForm.ScaleY(sy);
  }

  if (rz != .0f)
  {
    r.xForm.RotateZ(rz);
  }

  float zTemp = (r.xForm.t - m_mView.t).Dot(m_vForward);
  if (zTemp > m_zNear/* && zTemp < m_zFar*/) // if in front of camera
  {
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
  if (numVerts <= 0)
  {
    return;
  }
  numVerts *= XR::Sprite::kNumVertices;
  
  FloatBuffer* pCols(Renderer::AllocBuffer(sizeof(Color), numVerts));
  XR_ASSERT(SpriteRenderer, pCols != 0);
#if defined XR_SPRITE_RENDERER_PERSISTENT_STREAMS
  FloatBuffer* pUVs(&m_uvs);
  FloatBuffer* pVerts(&m_vertices);
#else
  FloatBuffer* pUVs(Renderer::AllocBuffer(sizeof(Vector2), numVerts));
  XR_ASSERT(SpriteRenderer, pUVs != 0);
  FloatBuffer* pVerts(Renderer::AllocBuffer(sizeof(Vector3), numVerts));
  XR_ASSERT(SpriteRenderer, pVerts != 0);
#endif
  
  int         vertexOffset(0);
  FloatBuffer colBatch;
  FloatBuffer uvBatch;
  FloatBuffer vertBatch;

  for (RecordList::const_iterator i0(m_pRecords->begin()),
    i1(m_pRecords->end()); i0 != i1;)
  {
    Material*  pMaterial(i0->pSprite->GetMaterial());

    // check for consecutive sprites with the same material
    int numSprites(1);

    RecordList::const_iterator j0(i0);
    while (++j0 != i1)
    {
      if (j0->pSprite->GetMaterial() != pMaterial)
      {
        break;
      }
      ++numSprites;
    }

    // calculate number of vertices
    numVerts = numSprites * XR::Sprite::kNumVertices;

    colBatch = FloatBuffer::Adapt(*pCols, vertexOffset, numVerts);
    uvBatch = FloatBuffer::Adapt(*pUVs, vertexOffset, numVerts);
    vertBatch = FloatBuffer::Adapt(*pVerts, vertexOffset, numVerts);

    // set the material
    Renderer::SetMaterial(i0->pSprite->GetMaterial());

    // render sprites
    while (i0 != j0)
    {
      pCols->Set(vertexOffset, i0->tint);
      pCols->Set(vertexOffset + 1, i0->tint);
      pCols->Set(vertexOffset + 2, i0->tint);
      pCols->Set(vertexOffset + 3, i0->tint);

      i0->pSprite->CopyUVsTo(vertexOffset, *pUVs);

      for (int i = 0; i < XR::Sprite::kNumVertices; ++i)
      {
        // TODO: optimize
        pVerts->Set(vertexOffset + i,
          i0->xForm.TransformVec(i0->pSprite->GetVertices().Get<Vector3>(i)));
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
    Renderer::DrawPrims(PrimType::TRI_LIST, &m_indices[0], numIndices);
  }
}

//==============================================================================
void  SpriteRenderer::Clear()
{
  XR_ASSERT(SpriteRenderer, m_pRecords != 0);
  m_pRecords->clear();
  m_pool.Flush();
}

} // XR
