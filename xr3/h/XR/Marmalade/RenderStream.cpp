#include <algorithm>
#include <IwGx.h>
#include "RenderStream.hpp"

namespace XR
{

//==============================================================================
const int  RenderStream::karElemSize[kMaxFormat] =
{
  sizeof(CIwFVec2),
  sizeof(CIwFVec3),
  sizeof(CIwColour)
};

RenderStream  RenderStream::s_zero(F_INVALID, 0);

//==============================================================================
int RenderStream::CalculateByteSize(Format fmt, int capacity)
{
  XR_ASSERT(RenderStream, fmt < F_INVALID);
  int bytes(capacity);
  bytes *= karElemSize[fmt];
  return bytes;
}

//==============================================================================
void* RenderStream::AllocateBuffer(Format fmt, int capacity)
{
  void* pBuffer(0);
  if(capacity > 0)
  {
    switch(fmt)
    {
    case  F_VECTOR2:
      pBuffer = new CIwFVec2[capacity];
      break;

    case  F_VECTOR3:
      pBuffer = new CIwFVec3[capacity];
      break;

    case  F_COLOR:
      pBuffer = new CIwColour[capacity];
      break;
    }
  }
  return pBuffer;
}

//==============================================================================
RenderStream::RenderStream()
: m_capacity(0),
  m_format(F_INVALID),
  m_pData(0),
  m_isOwner(false)
{} 

//==============================================================================
RenderStream::RenderStream(Format fmt, int capacity)
: m_capacity(capacity),
  m_format(fmt),
  m_pData(0),
  m_isOwner(false)
{
  Create(fmt, capacity);
}

//==============================================================================
RenderStream::RenderStream(Format fmt, int capacity, void* pData, bool deallocate)
: m_capacity(capacity),
  m_format(fmt),
  m_pData(pData),
  m_isOwner(deallocate)
{}

//==============================================================================
RenderStream::RenderStream(const RenderStream& rhs)
: m_capacity(rhs.GetCapacity()),
  m_format(rhs.GetFormat()),
  m_pData(rhs.GetData()),
  m_isOwner(rhs.m_isOwner)
{
  if(m_isOwner)
  {
    m_pData = AllocateBuffer(m_format, m_capacity);
    memcpy(m_pData, rhs.m_pData, CalculateByteSize(m_format, m_capacity));
  }
}

//==============================================================================
RenderStream::~RenderStream()
{
  Destroy();
}

//==============================================================================
void* RenderStream::GetData(int offset) const
{
  XR_ASSERT(RenderStream, m_format < F_INVALID);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    return static_cast<CIwFVec2*>(m_pData) + offset;

  case  F_VECTOR3:
    return static_cast<CIwFVec3*>(m_pData) + offset;

  case  F_COLOR:
    return static_cast<CIwColour*>(m_pData) + offset;
  }
  return m_pData;
}

//==============================================================================
Vector2  RenderStream::GetVector2(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  const CIwFVec2* p(static_cast<const CIwFVec2*>(m_pData));
  p += i;
  return Vector2(p->x, p->y);
}

//==============================================================================
Vector3  RenderStream::GetVector3(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  const CIwFVec3* p(static_cast<const CIwFVec3*>(m_pData));
  p += i;
  return Vector3(p->x, p->y, p->z);
}

//==============================================================================
Color  RenderStream::GetColor(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_COLOR);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  const CIwColour* p(static_cast<const CIwColour*>(m_pData));
  p += i;
  return Color(p->Get());
}

//==============================================================================
float RenderStream::GetX(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    return (static_cast<CIwFVec2*>(m_pData) + i)->x;
    break;

  case  F_VECTOR3:
    return (static_cast<CIwFVec3*>(m_pData) + i)->x;
    break;
  }
};

//==============================================================================
float RenderStream::GetY(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    return (static_cast<CIwFVec2*>(m_pData) + i)->y;
    break;

  case  F_VECTOR3:
    return (static_cast<CIwFVec3*>(m_pData) + i)->y;
    break;
  }
};

//==============================================================================
float RenderStream::GetZ(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  return (static_cast<CIwFVec3*>(m_pData) + i)->z;
};

//==============================================================================
void  RenderStream::Set(int i, const Vector2& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  CIwFVec2* p(static_cast<CIwFVec2*>(m_pData));
  p += i;
  p->x = v.x;
  p->y = v.y;
}

//==============================================================================
void  RenderStream::Set(int i, const Vector3& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  CIwFVec3* p(static_cast<CIwFVec3*>(m_pData));
  p += i;
  p->x = v.x;
  p->y = v.y;
  p->z = v.z;
}

//==============================================================================
void  RenderStream::Set(int i, const Color& c)
{
  XR_ASSERT(RenderStream, m_format == F_COLOR);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  CIwColour* p(static_cast<CIwColour*>(m_pData));
  p += i;
  p->Set(c.r * 0xff, c.g * 0xff, c.b * 0xff, c.a * 0xff);
}

//==============================================================================
void  RenderStream::SetX(int i, float x)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    (static_cast<CIwFVec2*>(m_pData) + i)->x = x;
    break;

  case  F_VECTOR3:
    (static_cast<CIwFVec3*>(m_pData) + i)->x = x;
    break;
  }
};

//==============================================================================
void  RenderStream::SetY(int i, float y)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    (static_cast<CIwFVec2*>(m_pData) + i)->y = y;
    break;

  case  F_VECTOR3:
    (static_cast<CIwFVec3*>(m_pData) + i)->y = y;
    break;
  }
};

//==============================================================================
void  RenderStream::SetZ(int i, float z)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  (static_cast<CIwFVec3*>(m_pData) + i)->z = z;
};

//==============================================================================
void  RenderStream::Add(int i, const Vector2& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  CIwFVec2* p(static_cast<CIwFVec2*>(m_pData));
  p += i;
  p->x += v.x;
  p->y += v.y;
}

//==============================================================================
void  RenderStream::Add(int i, const Vector3& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  CIwFVec3* p(static_cast<CIwFVec3*>(m_pData));
  p += i;
  p->x += v.x;
  p->y += v.y;
  p->z += v.z;
}

//==============================================================================
void  RenderStream::AddX(int i, float x)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    (static_cast<CIwFVec2*>(m_pData) + i)->x += x;
    break;

  case  F_VECTOR3:
    (static_cast<CIwFVec3*>(m_pData) + i)->x += x;
    break;
  }
};

//==============================================================================
void  RenderStream::AddY(int i, float y)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  switch(m_format)
  {
  case  F_VECTOR2:
    (static_cast<CIwFVec2*>(m_pData) + i)->y += y;
    break;

  case  F_VECTOR3:
    (static_cast<CIwFVec3*>(m_pData) + i)->y += y;
    break;
  }
};

//==============================================================================
void  RenderStream::AddZ(int i, float z)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, i >= 0);
  XR_ASSERT(RenderStream, i < m_capacity);
  (static_cast<CIwFVec3*>(m_pData) + i)->z += z;
};

//==============================================================================
void  RenderStream::Copy(const RenderStream& rhs, int readOffset,
  int readSize, int writeOffset)
{
  XR_ASSERT(RenderStream, rhs.m_format < F_INVALID);
  XR_ASSERT(RenderStream, m_format == rhs.m_format);
  XR_ASSERT(RenderStream, readOffset >= 0);
  if(readSize == SIZE_REST)
  {
    readSize = rhs.GetCapacity() - readOffset;
  }

  XR_ASSERT(RenderStream, readOffset + readSize <= rhs.m_capacity);
  XR_ASSERT(RenderStream, writeOffset >= 0);
  XR_ASSERT(RenderStream, writeOffset + readSize <= m_capacity);

  int bytesOffset(CalculateByteSize(m_format, writeOffset));
  int bytesSize(CalculateByteSize(m_format, rhs.m_capacity));
  memcpy(static_cast<char*>(m_pData) + bytesOffset,
    rhs.m_pData, bytesSize);
}

//==============================================================================
void  RenderStream::Adapt(const RenderStream& rhs, int readOffset,
  int readSize)
{
  Destroy();

  m_capacity = readSize;
  m_format = rhs.GetFormat();
  m_pData = rhs.GetData(readOffset);
}

//==============================================================================
void  RenderStream::Create(Format fmt, int capacity)
{
  XR_ASSERT(RenderStream, fmt < F_INVALID || capacity == 0);
  XR_ASSERT(RenderStream, capacity >= 0);
  Destroy();

  m_format = fmt;
  m_capacity = capacity;
  if(capacity > 0)
  {
    m_isOwner = true;

    switch(fmt)
    {
    case  F_VECTOR2:
      m_pData = new CIwFVec2[capacity];
      break;

    case  F_VECTOR3:
      m_pData = new CIwFVec3[capacity];
      break;

    case  F_COLOR:
      m_pData = new CIwColour[capacity];
      break;
    }
  }
}

//==============================================================================
void  RenderStream::Scale(float s, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch(m_format)
  {
  case  F_VECTOR2:
    {
      CIwFVec2* pStart(static_cast<CIwFVec2*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, Scaler(s));
      break;
    }

  case  F_VECTOR3:
    {
      CIwFVec3* pStart(static_cast<CIwFVec3*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, Scaler(s));
      break;
    }
  }
}

//==============================================================================
void  RenderStream::ScaleX(float sx, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch(m_format)
  {
  case  F_VECTOR2:
    {
      CIwFVec2* pStart(static_cast<CIwFVec2*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, ScalerX(sx));
      break;
    }

  case  F_VECTOR3:
    {
      CIwFVec3* pStart(static_cast<CIwFVec3*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, ScalerX(sx));
      break;
    }
  }
}

//==============================================================================
void  RenderStream::ScaleY(float sy, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch(m_format)
  {
  case  F_VECTOR2:
    {
      CIwFVec2* pStart(static_cast<CIwFVec2*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, ScalerY(sy));
      break;
    }

  case  F_VECTOR3:
    {
      CIwFVec3* pStart(static_cast<CIwFVec3*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, ScalerY(sy));
      break;
    }
  }
}

//==============================================================================
void  RenderStream::ScaleZ(float sz, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch(m_format)
  {
  case  F_VECTOR3:
    {
      CIwFVec3* pStart(static_cast<CIwFVec3*>(m_pData) + offset);
      std::for_each(pStart, pStart + size, ScalerZ(sz));
      break;
    }
  }
}

//==============================================================================
void  RenderStream::Translate(const Vector2& t, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  CIwFVec2* pStart(static_cast<CIwFVec2*>(m_pData) + offset);
  std::for_each(pStart, pStart + size,
    Translator<CIwFVec2>(CIwFVec2(t.x, t.y)));
}

//==============================================================================
void  RenderStream::Translate(const Vector3& t, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset <= m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if(size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  CIwFVec3* pStart(static_cast<CIwFVec3*>(m_pData) + offset);
  std::for_each(pStart, pStart + size,
    Translator<CIwFVec3>(CIwFVec3(t.x, t.y, t.z)));
}

//==============================================================================
void  RenderStream::Destroy()
{
  if(m_isOwner)
  {
    switch(m_format)
    {
    case  F_VECTOR2:
      delete[] static_cast<CIwFVec2*>(m_pData);
      break;

    case  F_VECTOR3:
      delete[] static_cast<CIwFVec3*>(m_pData);
      break;

    case  F_COLOR:
      delete[] static_cast<CIwColour*>(m_pData);
      break;
    }
    m_isOwner = false;
  }
  m_pData = 0;
  m_capacity = 0;
}

//==============================================================================
RenderStream& RenderStream::operator=(const RenderStream& rhs)
{
  RenderStream  temp(rhs);
  std::swap(m_pData, temp.m_pData);
  std::swap(m_isOwner, temp.m_isOwner);
  m_capacity = temp.m_capacity;
  m_format = temp.m_format;
  return *this;
}

} // XR