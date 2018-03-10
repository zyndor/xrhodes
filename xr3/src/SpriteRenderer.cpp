//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include "XR/IndexMesh.hpp"
#include "XR/SpriteRenderer.hpp"
#include "XR/VertexFormats.hpp"

namespace XR
{

//==============================================================================
SpriteRenderer::SpriteRenderer()
: m_pool(),
  m_records(nullptr)
{}

//==============================================================================
SpriteRenderer::~SpriteRenderer()
{
  m_pool.Pop();
  m_pool.Flush();

  m_records = nullptr;
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
  size_t nodeSize = Align<size_t>((sizeof(Record) + 2 * sizeof(void*)), 4);
  size_t memSize = Align<size_t>(sizeof(RecordList), 4) + nodeSize * (2 * numRecords + 1);
  m_pool.SetBuffer(memSize, true, 0);

  // place list at the beginning of pool memory. this includes the initial
  // nodes created to indicate beginning/end of list.
  void* pMemList(m_pool.Allocate(sizeof(RecordList)));
  m_records = new (pMemList)
    RecordList(XR::SharedPoolAllocator<Record>(m_pool));

  // push frame - flushes must not destroy the list.
  m_pool.Push();

  // Create index pattern.
  Gfx::Destroy(m_ibo);

  IndexMeshCore imc;
  imc.SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices, numRecords);
  m_ibo = Gfx::CreateIndexBuffer(imc.GetIndexBuffer(), 0);
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
void  SpriteRenderer::Add(Color tint, const XR::Sprite* sprite,
  const Vector3& position, float s, float rz, bool billboard)
{
  XR_ASSERT(SpriteRenderer, m_records->size() < (size_t)m_capacity);
  Record  r =
  {
    billboard ? Matrix(m_mView, position) : Matrix(position),
    tint,
    sprite
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
    RecordList::iterator iInsert(std::lower_bound(m_records->begin(),
      m_records->end(), r, Record::Compare));
    m_records->insert(iInsert, r);
  }
}

//==============================================================================
void  SpriteRenderer::Add(Color tint, const XR::Sprite* sprite,
  const Vector3& position, float sx, float sy, float rz, bool billboard)
{
  XR_ASSERT(SpriteRenderer, m_records->size() < (size_t)m_capacity);
  Record  r =
  {
    billboard ? Matrix(m_mView, position) : Matrix(position),
    tint,
    sprite
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
    RecordList::iterator iInsert(std::lower_bound(m_records->begin(),
      m_records->end(), r, Record::Compare));
    m_records->insert(iInsert, r);
  }
}

//==============================================================================
void  SpriteRenderer::Render()
{
  if (m_records->empty())
  {
    return;
  }

  uint32_t numVertices = static_cast<uint32_t>(m_records->size()) * XR::Sprite::kNumVertices;
  FloatBuffer vertices(sizeof(Vertex), numVertices);
  auto vertsHead = vertices.Get<Vertex>();

  int vertexOffset = 0;
  for (RecordList::const_iterator i0(m_records->begin()),
    i1(m_records->end()); i0 != i1;)
  {
    auto const&  pMaterial(i0->pSprite->GetMaterial());

    // check for consecutive sprites with the same material
    uint32_t numSprites = 1;

    RecordList::const_iterator j0 = i0;
    while (++j0 != i1)
    {
      if (j0->pSprite->GetMaterial() != pMaterial)
      {
        break;
      }
      ++numSprites;
    }

    // calculate number of vertices
    numVertices = numSprites * XR::Sprite::kNumVertices;

    auto verts = vertsHead;

    // set the material
    i0->pSprite->GetMaterial()->Apply();

    // render sprites
    while (i0 != j0)
    {
      verts[0].color0 = i0->tint;
      verts[1].color0 = i0->tint;
      verts[2].color0 = i0->tint;
      verts[3].color0 = i0->tint;

      i0->pSprite->CopyVerticesTo(verts);
      for (int i = 0; i < XR::Sprite::kNumVertices; ++i)
      {
        verts[i].pos = i0->xForm.TransformVec(verts[i].pos);
      }

      ++i0;
      verts += Sprite::kNumVertices;
    }

    // indices
    uint32_t numIndices = numSprites * Sprite::kNumIndices;

    // render
    auto vertsBytes = reinterpret_cast<uint8_t const*>(vertsHead);
    Gfx::VertexBufferHandle vbo =
      Gfx::CreateVertexBuffer(XR::Vertex::Formats::GetHandle<Vertex>(),
        { numVertices * sizeof(Vertex), vertsBytes }, Gfx::F_BUFFER_NONE);
    Gfx::Draw(vbo, m_ibo, PrimType::TRI_LIST, 0, numIndices);
    Gfx::Destroy(vbo);

    vertexOffset += numVertices;
  }
}

//==============================================================================
void  SpriteRenderer::Clear()
{
  XR_ASSERT(SpriteRenderer, m_records != 0);
  m_records->clear();
  m_pool.Flush();
}

} // XR
