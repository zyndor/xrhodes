//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "FloatBuffer.hpp"

namespace XR
{

//=============================================================================
FloatBuffer FloatBuffer::Adapt(FloatBuffer& other, size_t offset, size_t size)
{
  return FloatBuffer(other, offset, size);
}

//=============================================================================
FloatBuffer::FloatBuffer()
: m_pDataOwner(nullptr),
  m_numDependents(0),
  m_elemSize(0),
  m_numElems(0),
  m_parData(nullptr)
{}

//=============================================================================
FloatBuffer::FloatBuffer(size_t elemSize, size_t numElems)
: m_pDataOwner(nullptr),
  m_numDependents(0),
  m_elemSize(elemSize),
  m_numElems(numElems),
  m_parData(AllocateBuffer(elemSize, numElems))
{}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer const & other)  // copy constructor
: m_pDataOwner(other.m_pDataOwner),
  m_numDependents(0),
  m_elemSize(other.m_elemSize),
  m_numElems(other.m_numElems),
  m_parData(other.m_parData)
{
  if (m_pDataOwner == nullptr)
  {
    m_parData = AllocateBuffer(m_elemSize, m_numElems);
    std::copy(other.m_parData, other.m_parData + (m_elemSize * m_numElems), m_parData);
  }
  else
  {
    ++m_pDataOwner->m_numDependents;
  }
}

//=============================================================================
FloatBuffer::~FloatBuffer()
{
  ReleaseData();
}

//=============================================================================
void FloatBuffer::SetSize(size_t elemSize, size_t numElems)
{
  ReleaseData();
  m_elemSize = elemSize;
  m_numElems = numElems;
  m_parData = AllocateBuffer(elemSize, numElems);
}

//=============================================================================
void FloatBuffer::Move(FloatBuffer&& other)
{
  ReleaseData();

  XR_ASSERT(FloatBuffer, other.m_numDependents == 0);
  m_pDataOwner = other.m_pDataOwner;
  other.m_pDataOwner = nullptr;

  m_numDependents = 0;

  m_elemSize = other.m_elemSize;
  m_numElems = other.m_numElems;
  m_parData = other.m_parData;
  other.m_parData = nullptr;
}

//=============================================================================
void FloatBuffer::Copy(FloatBuffer const & other)
{
  *this = other;
}

//=============================================================================
bool FloatBuffer::Own()
{
  const bool result = m_pDataOwner != nullptr;
  if (result)
  {
    FloatBuffer temp(*this);
    ReleaseData();
    SetSize(temp.m_elemSize, temp.m_numElems);
    std::copy(temp.m_parData, temp.m_parData + temp.m_elemSize * temp.m_numElems,
      m_parData);
  }
  return result;
}

//=============================================================================
float * FloatBuffer::AllocateBuffer(size_t elemSize, size_t numElems)
{
  float* buffer = nullptr;
  const size_t totalSize = elemSize * numElems;
  if (totalSize > 0)
  {
    buffer = new float[totalSize];
  }
  return buffer;
}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer & other, size_t offset, size_t size)  // adapt constructor
: m_pDataOwner(&other),
  m_numDependents(0),
  m_elemSize(other.m_elemSize),
  m_numElems(other.ResolveSize(offset, size)),
  m_parData(other.m_parData + offset * other.m_elemSize)
{
  while (m_pDataOwner->m_pDataOwner)
  {
    m_pDataOwner = m_pDataOwner->m_pDataOwner;
  }
  ++m_pDataOwner->m_numDependents;
}

//=============================================================================
void FloatBuffer::ReleaseData()
{
  if (m_pDataOwner != nullptr)
  {
    DetachFromOwner();
  }
  else
  {
    XR_ASSERT(FloatBuffer, m_numDependents == 0);
    delete[] m_parData;
  }

  m_numElems = 0;
  m_elemSize = 0;
  m_parData = nullptr;
}

//=============================================================================
void FloatBuffer::DetachFromOwner()
{
  XR_ASSERT(FloatBuffer, m_pDataOwner != nullptr);
  --m_pDataOwner->m_numDependents;
  m_pDataOwner = nullptr;
}

//=============================================================================
size_t FloatBuffer::ResolveSize(size_t offset, size_t size) const
{
  XR_ASSERT(FloatBuffer, offset <= m_numElems);
  XR_ASSERT(FloatBuffer, size <= m_numElems - offset);
  return size == kSizeRest ? m_numElems - offset : size;
}

}