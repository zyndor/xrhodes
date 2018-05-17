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
  m_data(nullptr),
  m_adapted(nullptr),
  m_ownData(false),
  m_numDependents(0)
{}

//=============================================================================
FloatBuffer::FloatBuffer(uint32_t elemSize, uint32_t numElems, float* parBuffer)
: m_elemSizeFloats(elemSize),
  m_numElems(numElems),
  m_data(parBuffer),
  m_adapted(nullptr),
  m_ownData(parBuffer == nullptr),
  m_numDependents(0)
{
  if (parBuffer == nullptr)
  {
    m_data = AllocateBuffer(elemSize, numElems);
  }
}

//=============================================================================
FloatBuffer::FloatBuffer(FloatBuffer const & other)  // copy constructor
: m_elemSizeFloats(other.m_elemSizeFloats),
  m_numElems(other.m_numElems),
  m_data(other.m_data),
  m_adapted(other.m_adapted),
  m_ownData(other.m_ownData),
  m_numDependents(0)
{
  if (m_adapted)
  {
    ++m_adapted->m_numDependents;
  }
  else
  {
    if (m_ownData)
    {
      m_data = AllocateBuffer(m_elemSizeFloats, m_numElems);
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
  m_data = ownData ? AllocateBuffer(elemSize, numElems) : parBuffer;
  m_ownData = ownData;
}

//=============================================================================
void FloatBuffer::Move(FloatBuffer&& other)
{
  ReleaseData();

  XR_ASSERT(FloatBuffer, other.m_numDependents == 0);
  m_adapted = other.m_adapted;
  other.m_adapted = nullptr;

  m_ownData = other.m_ownData;
  other.m_ownData = false;
  // if we're owning data, we're must not be adapted.
  XR_ASSERT(FloatBuffer, !m_ownData || (m_ownData && m_adapted == nullptr));

  m_numDependents = 0;

  m_elemSizeFloats = other.m_elemSizeFloats;
  m_numElems = other.m_numElems;
  m_data = other.m_data;
  other.m_data = nullptr;
}

//=============================================================================
bool FloatBuffer::Own()
{
  const bool result = !m_ownData && m_adapted != nullptr;
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
  if (m_adapted != nullptr)
  {
    XR_ASSERT(FloatBuffer, !m_ownData);
    DetachFromOwner();
  }
  else
  {
    XR_ASSERT(FloatBuffer, m_numDependents == 0);
    if (m_ownData)
    {
      delete[] m_data;
      m_ownData = false;
    }
  }

  m_numElems = 0;
  m_elemSizeFloats = 0;
  m_data = nullptr;
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
  XR_ASSERT(FloatBuffer, elemSize % sizeof(float) == 0);
  float* buffer = nullptr;
  const uint32_t totalSize = elemSize * numElems / sizeof(float);
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
  m_data(other.m_data + offset * other.m_elemSizeFloats),
  m_adapted(other.m_adapted ? other.m_adapted : &other),
  m_ownData(false),
  m_numDependents(0)
{
  ++m_adapted->m_numDependents;
}

//=============================================================================
void FloatBuffer::CopyData(FloatBuffer const & other)
{
  XR_ASSERT(FloatBuffer, m_elemSizeFloats == other.m_elemSizeFloats);
  XR_ASSERT(FloatBuffer, m_numElems == other.m_numElems);
  std::copy(other.m_data, other.m_data + (other.m_elemSizeFloats * other.m_numElems),
    m_data);
}

//=============================================================================
void FloatBuffer::DetachFromOwner()
{
  XR_ASSERT(FloatBuffer, m_adapted != nullptr);
  --m_adapted->m_numDependents;
  m_adapted = nullptr;
}

//=============================================================================
uint32_t FloatBuffer::ResolveSize(uint32_t offset, uint32_t size) const
{
  XR_ASSERT(FloatBuffer, offset <= m_numElems);
  XR_ASSERT(FloatBuffer, size == kSizeRest || offset + size <= m_numElems);
  return size == kSizeRest ? m_numElems - offset : size;
}

} // XR
