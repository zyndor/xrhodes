#include "RenderStream.hpp"
#include <algorithm>
#include <SDL2/SDL_opengl.h>

namespace XR
{

//==============================================================================
struct  Vec2gl
{
  GLfloat x, y;
  
  Vec2gl& operator*=(float s)
  {
    x *= s;
    y *= s;
    return *this;
  }
};

//==============================================================================
struct  Vec3gl: Vec2gl
{
  GLfloat z;

  Vec3gl& operator*=(float s)
  {
    Vec2gl::operator*=(s);
    z *= s;
    return *this;
  }
};

//==============================================================================
const int  RenderStream::karElemSize[kMaxFormat] =
{
  sizeof(GLfloat) * 2,
  sizeof(GLfloat) * 3,
  sizeof(GLfloat) * 4
};

RenderStream  RenderStream::s_zero(F_INVALID, 0, 0, false);

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
  if (capacity > 0)
  {
    switch (fmt)
    {
    case  F_VECTOR2:
      pBuffer = new GLfloat[capacity * 2];
      break;

    case  F_VECTOR3:
      pBuffer = new GLfloat[capacity * 3];
      break;

    case  F_COLOR:
      pBuffer = new GLfloat[capacity * 4];
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
  if (m_isOwner)
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
  switch (m_format)
  {
  case  F_VECTOR2:
    return static_cast<GLfloat*>(m_pData) + offset * 2;

  case  F_VECTOR3:
    return static_cast<GLfloat*>(m_pData) + offset * 3;

  case  F_COLOR:
    return static_cast<GLfloat*>(m_pData) + offset * 4;
  }
  return m_pData;
}

//==============================================================================
Vector2  RenderStream::GetVector2(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  const GLfloat* p(static_cast<const GLfloat*>(m_pData));
  p += i * 2;
  return Vector2(p);
}

//==============================================================================
Vector3  RenderStream::GetVector3(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  const GLfloat* p(static_cast<const GLfloat*>(m_pData));
  p += i * 3;
  return Vector3(p);
}

//==============================================================================
Color  RenderStream::GetColor(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_COLOR);
  const GLfloat* p(static_cast<const GLfloat*>(m_pData));
  p += i * 4;
  return Color(p);
}

//==============================================================================
float RenderStream::GetX(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    return *(static_cast<GLfloat*>(m_pData) + i * 2 + VX);
    
  case  F_VECTOR3:
    return *(static_cast<GLfloat*>(m_pData) + i * 3 + VX);
  }
}

//==============================================================================
float RenderStream::GetY(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    return *(static_cast<GLfloat*>(m_pData) + (i * 2) + VY);
    
  case  F_VECTOR3:
    return *(static_cast<GLfloat*>(m_pData) + (i * 3) + VY);
  }
}

//==============================================================================
float RenderStream::GetZ(int i) const
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  return *(static_cast<GLfloat*>(m_pData) + (i * 3) + VZ);
}

//==============================================================================
void  RenderStream::Set(int i, const Vector2& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  GLfloat* p(static_cast<GLfloat*>(m_pData));
  p += i * 2;
  p[0] = v.x;
  p[1] = v.y;
}

//==============================================================================
void  RenderStream::Set(int i, const Vector3& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  GLfloat* p(static_cast<GLfloat*>(m_pData));
  p += i * 3;
  p[0] = v.x;
  p[1] = v.y;
  p[2] = v.z;
}

//==============================================================================
void  RenderStream::Set(int i, const Color& c)
{
  XR_ASSERT(RenderStream, m_format == F_COLOR);
  GLfloat* p(static_cast<GLfloat*>(m_pData));
  p += i * 4;
  p[0] = c.r;
  p[1] = c.g;
  p[2] = c.b;
  p[3] = c.a;
}

//==============================================================================
void  RenderStream::SetX(int i, float x)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    *(static_cast<GLfloat*>(m_pData) + i * 2 + VX) = x;
    break;
    
  case  F_VECTOR3:
    *(static_cast<GLfloat*>(m_pData) + i * 3 + VX) = x;
    break;
  }
}

//==============================================================================
void  RenderStream::SetY(int i, float y)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    *(static_cast<GLfloat*>(m_pData) + (i * 2) + VY) = y;
    break;
    
  case  F_VECTOR3:
    *(static_cast<GLfloat*>(m_pData) + (i * 3) + VY) = y;
    break;
  }
}

//==============================================================================
void  RenderStream::SetZ(int i, float z)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  *(static_cast<GLfloat*>(m_pData) + (i * 3) + VZ) = z;
}

//==============================================================================
void  RenderStream::Add(int i, const Vector2& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  GLfloat* p(static_cast<GLfloat*>(m_pData));
  p += i * 2;
  p[0] += v.x;
  p[1] += v.y;
}

//==============================================================================
void  RenderStream::Add(int i, const Vector3& v)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  GLfloat* p(static_cast<GLfloat*>(m_pData));
  p += i * 3;
  p[0] += v.x;
  p[1] += v.y;
  p[2] += v.z;
}

//==============================================================================
void  RenderStream::AddX(int i, float x)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    *(static_cast<GLfloat*>(m_pData) + i * 2 + VX) += x;
    break;
    
  case  F_VECTOR3:
    *(static_cast<GLfloat*>(m_pData) + i * 3 + VX) += x;
    break;
  }
}

//==============================================================================
void  RenderStream::AddY(int i, float y)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  switch (m_format)
  {
  case  F_VECTOR2:
    *(static_cast<GLfloat*>(m_pData) + (i * 2) + VY) += y;
    break;
    
  case  F_VECTOR3:
    *(static_cast<GLfloat*>(m_pData) + (i * 3) + VY) += y;
    break;
  }
}

//==============================================================================
void  RenderStream::AddZ(int i, float z)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  *(static_cast<GLfloat*>(m_pData) + (i * 3) + VZ) += z;
}

//==============================================================================
void  RenderStream::Copy(const RenderStream& rhs, int readOffset, int readSize,
  int writeOffset)
{
  XR_ASSERT(RenderStream, rhs.m_format < F_INVALID);
  XR_ASSERT(RenderStream, m_format == rhs.m_format);
  XR_ASSERT(RenderStream, readOffset >= 0);
  if (readSize == SIZE_REST)
  {
    readSize = rhs.GetCapacity() - readOffset;
  }
  
  XR_ASSERT(RenderStream, readSize >= 0);
  XR_ASSERT(RenderStream, readOffset + readSize <= rhs.GetCapacity());
  XR_ASSERT(RenderStream, writeOffset >= 0);
  XR_ASSERT(RenderStream, writeOffset + readSize <= GetCapacity());
  memcpy(GetData(writeOffset), rhs.GetData(readOffset),
    CalculateByteSize(m_format, readSize));
}

//==============================================================================
void  RenderStream::Adapt(const RenderStream& rhs, int readOffset,
        int readSize)
{
  if (readSize == SIZE_REST)
  {
    readSize = rhs.GetCapacity() - readOffset;
  }
  XR_ASSERT(RenderStream, readSize >= 0);
  XR_ASSERT(RenderStream, readOffset + readSize <= rhs.GetCapacity());
  
  Destroy();
  
  m_capacity = readSize;
  m_format = rhs.GetFormat();
  m_pData = rhs.GetData(readOffset);
  m_isOwner = false;
}

//==============================================================================
void  RenderStream::Create(Format fmt, int capacity)
{
  XR_ASSERT(RenderStream, fmt < F_INVALID);
  XR_ASSERT(RenderStream, capacity >= 0);
  Destroy();
  if (capacity > 0)
  {
    m_isOwner = true;
    m_pData = AllocateBuffer(fmt, capacity);
  }
  m_format = fmt;
  m_capacity = capacity;
}

//==============================================================================
void  RenderStream::Destroy()
{
  if (m_isOwner)
  {
    switch (m_format)
    {
    case  F_VECTOR2:
    case  F_VECTOR3:
    case  F_COLOR:
      delete[] static_cast<GLfloat*>(m_pData);
      break;
    }
    m_isOwner = false;
  }
  m_pData = 0;
  m_capacity = 0;
}

//==============================================================================
void  RenderStream::Scale(float s, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch (m_format)
  {
  case  F_VECTOR2:
    std::for_each(static_cast<Vec2gl*>(GetData(offset)),
      static_cast<Vec2gl*>(GetData(offset)) + size, Scaler(s));
    break;

  case  F_VECTOR3:
    std::for_each(static_cast<Vec3gl*>(GetData(offset)),
      static_cast<Vec3gl*>(GetData(offset)) + size, Scaler(s));
    break;
  }
}

//==============================================================================
void  RenderStream::ScaleX(float sx, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch (m_format)
  {
  case  F_VECTOR2:
    std::for_each(static_cast<Vec2gl*>(GetData(offset)),
      static_cast<Vec2gl*>(GetData(offset)) + size, ScalerX(sx));
    break;

  case  F_VECTOR3:
    std::for_each(static_cast<Vec3gl*>(GetData(offset)),
      static_cast<Vec3gl*>(GetData(offset)) + size, ScalerX(sx));
    break;
  }
}

//==============================================================================
void  RenderStream::ScaleY(float sy, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2 || m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch (m_format)
  {
  case  F_VECTOR2:
    std::for_each(static_cast<Vec2gl*>(GetData(offset)),
      static_cast<Vec2gl*>(GetData(offset)) + size, ScalerY(sy));
    break;

  case  F_VECTOR3:
    std::for_each(static_cast<Vec3gl*>(GetData(offset)),
      static_cast<Vec3gl*>(GetData(offset)) + size, ScalerY(sy));
    break;
  }
}

//==============================================================================
void  RenderStream::ScaleZ(float sz, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  switch (m_format)
  {
  case  F_VECTOR3:
    std::for_each(static_cast<Vec3gl*>(GetData(offset)),
      static_cast<Vec3gl*>(GetData(offset)) + size, ScalerZ(sz));
    break;
  }
}

//==============================================================================
void  RenderStream::Translate(const Vector2& t, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR2);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  GLfloat*  i0(static_cast<GLfloat*>(m_pData) + offset * 2);
  GLfloat*  i1(static_cast<GLfloat*>(m_pData) + (offset + size) * 2);
  while (i0 != i1)
  {
    i0[VX] += t.x;
    i0[VY] += t.y;
    i0 += 2;
  }
}

//==============================================================================
void  RenderStream::Translate(const Vector3& t, int offset, int size)
{
  XR_ASSERT(RenderStream, m_format == F_VECTOR3);
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, offset < m_capacity);
  XR_ASSERT(RenderStream, size >= SIZE_REST);
  XR_ASSERT(RenderStream, size == SIZE_REST || offset + size <= m_capacity);
  if (size == SIZE_REST)
  {
    size = m_capacity - offset;
  }

  GLfloat*  i0(static_cast<GLfloat*>(m_pData) + offset * 3);
  GLfloat*  i1(static_cast<GLfloat*>(m_pData) + (offset + size) * 3);
  while (i0 != i1)
  {
    i0[VX] += t.x;
    i0[VY] += t.y;
    i0[VZ] += t.z;
    i0 += 2;
  }
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