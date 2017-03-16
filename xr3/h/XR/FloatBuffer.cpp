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
: m_elemSize(0),
  m_numElems(0),
  m_parData(nullptr),
  m_pAdapted(nullptr),
  m_numDependents(0)
{}

//=============================================================================
FloatBuffer::FloatBuffer(size_t elemSize, size_t numElems)
: m_elemSize(elemSize),
  m_numElems(numElems),
  m_parData(AllocateBuffer(elemSize, numElems)),
  m_pAdapted(nullptr),
  m_numDependents(0)
{}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer const & other)  // copy constructor
: m_elemSize(other.m_elemSize),
  m_numElems(other.m_numElems),
  m_parData(other.m_parData),
  m_pAdapted(other.m_pAdapted),
  m_numDependents(0)
{
  if (m_pAdapted == nullptr)
  {
    m_parData = AllocateBuffer(m_elemSize, m_numElems);
    CopyData(other);
  }
  else
  {
    ++m_pAdapted->m_numDependents;
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
  m_pAdapted = other.m_pAdapted;
  other.m_pAdapted = nullptr;

  m_numDependents = 0;

  m_elemSize = other.m_elemSize;
  m_numElems = other.m_numElems;
  m_parData = other.m_parData;
  other.m_parData = nullptr;
}

//=============================================================================
bool FloatBuffer::Own()
{
  const bool result = m_pAdapted != nullptr;
  if (result)
  {
    FloatBuffer temp(*this);
    ReleaseData();
    SetSize(temp.m_elemSize, temp.m_numElems);
    CopyData(temp);
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
: m_pAdapted(other.m_pAdapted ? other.m_pAdapted : &other),
  m_numDependents(0),
  m_elemSize(other.m_elemSize),
  m_numElems(other.ResolveSize(offset, size)),
  m_parData(other.m_parData + offset * other.m_elemSize)
{
  ++m_pAdapted->m_numDependents;
}

//=============================================================================
void FloatBuffer::CopyData(FloatBuffer const & other)
{
  XR_ASSERT(FloatBuffer, m_elemSize == other.m_elemSize);
  XR_ASSERT(FloatBuffer, m_numElems == other.m_numElems);
  std::copy(other.m_parData, other.m_parData + (other.m_elemSize * other.m_numElems),
    m_parData);
}

//=============================================================================
void FloatBuffer::ReleaseData()
{
  if (m_pAdapted != nullptr)
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
  XR_ASSERT(FloatBuffer, m_pAdapted != nullptr);
  --m_pAdapted->m_numDependents;
  m_pAdapted = nullptr;
}

//=============================================================================
size_t FloatBuffer::ResolveSize(size_t offset, size_t size) const
{
  XR_ASSERT(FloatBuffer, offset <= m_numElems);
  XR_ASSERT(FloatBuffer, size <= m_numElems - offset);
  return size == kSizeRest ? m_numElems - offset : size;
}

}