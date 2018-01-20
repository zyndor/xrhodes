//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/FloatBuffer.hpp"

namespace XR
{

//=============================================================================
FloatBuffer FloatBuffer::Adapt(FloatBuffer& other, uint32_t offset, uint32_t size)
{
  XR_ASSERT(FloatBuffer, offset < other.m_numElems);
  return FloatBuffer(other, offset, size);
}

//=============================================================================
FloatBuffer::FloatBuffer()
: m_elemSizeFloats(0),
  m_numElems(0),
  m_parData(nullptr),
  m_pAdapted(nullptr),
  m_ownData(false),
  m_numDependents(0)
{}

//=============================================================================
FloatBuffer::FloatBuffer(uint32_t elemSize, uint32_t numElems, float* parBuffer)
: m_elemSizeFloats(elemSize),
  m_numElems(numElems),
  m_parData(parBuffer),
  m_pAdapted(nullptr),
  m_ownData(parBuffer == nullptr),
  m_numDependents(0)
{
  if (parBuffer == nullptr)
  {
    m_parData = AllocateBuffer(elemSize, numElems);
  }
}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer const & other)  // copy constructor
: m_elemSizeFloats(other.m_elemSizeFloats),
  m_numElems(other.m_numElems),
  m_parData(other.m_parData),
  m_pAdapted(other.m_pAdapted),
  m_ownData(other.m_ownData),
  m_numDependents(0)
{
  if (m_pAdapted)
  {
    ++m_pAdapted->m_numDependents;
  }
  else
  {
    if (m_ownData)
    {
      m_parData = AllocateBuffer(m_elemSizeFloats, m_numElems);
      CopyData(other);
    }
  }
}

//=============================================================================
FloatBuffer::~FloatBuffer()
{
  ReleaseData();
}

//=============================================================================
void FloatBuffer::SetBuffer(uint32_t elemSize, uint32_t numElems, float* parBuffer)
{
  XR_ASSERT(FloatBuffer, elemSize % sizeof(float) == 0);
  ReleaseData();
  m_elemSizeFloats = elemSize / sizeof(float);
  m_numElems = numElems;

  const bool ownData = parBuffer == nullptr;
  m_parData = ownData ? AllocateBuffer(elemSize, numElems) : parBuffer;
  m_ownData = ownData;
}

//=============================================================================
void FloatBuffer::Move(FloatBuffer&& other)
{
  ReleaseData();

  XR_ASSERT(FloatBuffer, other.m_numDependents == 0);
  m_pAdapted = other.m_pAdapted;
  other.m_pAdapted = nullptr;

  m_ownData = other.m_ownData;
  other.m_ownData = false;
  // if we're owning data, we're must not be adapted.
  XR_ASSERT(FloatBuffer, !m_ownData || (m_ownData && m_pAdapted == nullptr));

  m_numDependents = 0;

  m_elemSizeFloats = other.m_elemSizeFloats;
  m_numElems = other.m_numElems;
  m_parData = other.m_parData;
  other.m_parData = nullptr;
}

//=============================================================================
bool FloatBuffer::Own()
{
  const bool result = !m_ownData && m_pAdapted != nullptr;
  if (result)
  {
    FloatBuffer temp(*this);
    ReleaseData();
    SetBuffer(temp.m_elemSizeFloats, temp.m_numElems);
    CopyData(temp);
  }
  return result;
}

//=============================================================================
void FloatBuffer::ReleaseData()
{
  if (m_pAdapted != nullptr)
  {
    XR_ASSERT(FloatBuffer, !m_ownData);
    DetachFromOwner();
  }
  else
  {
    XR_ASSERT(FloatBuffer, m_numDependents == 0);
    if (m_ownData)
    {
      delete[] m_parData;
      m_ownData = false;
    }
  }

  m_numElems = 0;
  m_elemSizeFloats = 0;
  m_parData = nullptr;
}

//=============================================================================
FloatBuffer& FloatBuffer::operator=(FloatBuffer const& rhs)
{
  FloatBuffer temp(rhs);
  ReleaseData();
  Move(std::move(temp));

  return *this;
}

//=============================================================================
float * FloatBuffer::AllocateBuffer(uint32_t elemSize, uint32_t numElems)
{
  float* buffer = nullptr;
  const uint32_t totalSize = elemSize * numElems;
  if (totalSize > 0)
  {
    buffer = new float[totalSize];
  }
  return buffer;
}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer & other, uint32_t offset, uint32_t size)  // adapt constructor
: m_elemSizeFloats(other.m_elemSizeFloats),
  m_numElems(other.ResolveSize(offset, size)),
  m_parData(other.m_parData + offset * other.m_elemSizeFloats),
  m_pAdapted(other.m_pAdapted ? other.m_pAdapted : &other),
  m_ownData(false),
  m_numDependents(0)
{
  ++m_pAdapted->m_numDependents;
}

//=============================================================================
void FloatBuffer::CopyData(FloatBuffer const & other)
{
  XR_ASSERT(FloatBuffer, m_elemSizeFloats == other.m_elemSizeFloats);
  XR_ASSERT(FloatBuffer, m_numElems == other.m_numElems);
  std::copy(other.m_parData, other.m_parData + (other.m_elemSizeFloats * other.m_numElems),
    m_parData);
}

//=============================================================================
void FloatBuffer::DetachFromOwner()
{
  XR_ASSERT(FloatBuffer, m_pAdapted != nullptr);
  --m_pAdapted->m_numDependents;
  m_pAdapted = nullptr;
}

//=============================================================================
uint32_t FloatBuffer::ResolveSize(uint32_t offset, uint32_t size) const
{
  XR_ASSERT(FloatBuffer, offset <= m_numElems);
  XR_ASSERT(FloatBuffer, size == kSizeRest || offset + size <= m_numElems);
  return size == kSizeRest ? m_numElems - offset : size;
}

} // XR
