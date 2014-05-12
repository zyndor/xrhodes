//
// Nuclear Heart Games
// XRhodes
//
// RenderStream.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    01/10/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_RENDERSTREAM_HPP
#define XR_RENDERSTREAM_HPP

#include "vectypes.hpp"
#include "Color.hpp"

namespace XR
{

//==============================================================================
class RenderStream
{
public:
  // types
  enum  Format
  {
    F_VECTOR2,
    F_VECTOR3,
    F_COLOR,
    kMaxFormat,
    F_INVALID = kMaxFormat
  };

  enum
  {
    SIZE_REST = -1
  };

  struct  Scaler
  {
    Scaler(float s_)
    : s(s_)
    {}

    template <typename T>
    void  operator()(T& t)
    {
      t *= s;
    }

    float s;
  };

  struct  ScalerX:  Scaler
  {
    ScalerX(float s)
    : Scaler(s)
    {}

    template <typename T>
    void  operator()(T& t)
    {
      t.x *= s;
    }
  };
  
  struct  ScalerY:  Scaler
  {
    ScalerY(float s)
    : Scaler(s)
    {}

    template <typename T>
    void  operator()(T& t)
    {
      t.y *= s;
    }
  };
  
  struct  ScalerZ:  Scaler
  {
    ScalerZ(float s)
    : Scaler(s)
    {}

    template <typename T>
    void  operator()(T& t)
    {
      t.z *= s;
    }
  };

  template  <class T>
  struct  Translator
  {
    Translator(const T& t_)
    : t(t_)
    {}

    void  operator()(T& v)
    {
      v += t;
    }

    T t;
  };

  // static
  static const int  karElemSize[kMaxFormat];

  static RenderStream  s_zero;

  static int    CalculateByteSize(Format fmt, int capacity);
  static void*  AllocateBuffer(Format fmt, int capacity);

  // structors
  RenderStream();
  RenderStream(Format fmt, int capacity);
  RenderStream(Format fmt, int capacity, void* pData, bool deallocate);
  RenderStream(const RenderStream& rhs);
  ~RenderStream();

  // general
  Format    GetFormat() const;
  int       GetCapacity() const;
  void*     GetData() const;
  void*     GetData(int offset) const;

  void      Create(Format fmt, int capacity);
  
  Vector2   GetVector2(int i) const;
  Vector3   GetVector3(int i) const;
  Color     GetColor(int i) const;

  float     GetX(int i) const;
  float     GetY(int i) const;
  float     GetZ(int i) const;

  void      Set(int i, const Vector2& v); 
  void      Set(int i, const Vector3& v); 
  void      Set(int i, const Color& c);
  
  void      SetX(int i, float x);
  void      SetY(int i, float y);
  void      SetZ(int i, float z);

  void      Add(int i, const Vector2& v); 
  void      Add(int i, const Vector3& v); 

  void      AddX(int i, float x);
  void      AddY(int i, float y);
  void      AddZ(int i, float z);

  ///@brief Copies @a readSize elements from @a rhs at @a readOffset, into
  ///       this at @a writeOffset.
  ///@note  Offsets and size must match both streams.
  void      Copy(const RenderStream& rhs, int readOffset = 0,
              int readSize = SIZE_REST, int writeOffset = 0);

  ///@brief Adjusts the format and contents and capacity of this stream to be
  ///       @a readSize elements of @a rhs at @a readOffset.
  ///@note  Offset and size must match @a rhs.
  ///@note  The contents of this are discarded (and if owned, deallocated).
  ///@note  Not a copy - modifying the elements will modify the original.
  void      Adapt(const RenderStream& rhs, int readOffset = 0,
              int readSize = SIZE_REST);
  
  ///@return  A renderstream object with the format of this stream, and contents
  ///         being @a size elements, at @a offset of this.
  ///@note    Offset and size must match.
  ///@note    Not a copy - modifying the elements will modify the original.
  RenderStream  SubStream(int offset, int size) const;

  void      Scale(float s, int offset = 0, int size = SIZE_REST); // F_VECTOR2 & 3
  void      ScaleX(float sx, int offset = 0, int size = SIZE_REST); // F_VECTOR2 & 3
  void      ScaleY(float sy, int offset = 0, int size = SIZE_REST); // F_VECTOR2 & 3
  void      ScaleZ(float sz, int offset = 0, int size = SIZE_REST); // F_VECTOR3 only
  void      Translate(const Vector2& t, int offset = 0, int size = SIZE_REST);
  void      Translate(const Vector3& t, int offset = 0, int size = SIZE_REST);
  
  void      Destroy();

  // operator overloads
  RenderStream& operator=(const RenderStream& rhs);

protected:
  // data
  int       m_capacity;
  Format    m_format;
  void*     m_pData;
  bool      m_isOwner;
};

//==============================================================================
//  implementation
//==============================================================================
inline
RenderStream::Format  RenderStream::GetFormat() const
{
  return m_format;
}

//==============================================================================
inline
int RenderStream::GetCapacity() const
{
  return m_capacity;
}

//==============================================================================
inline
void* RenderStream::GetData() const
{
  return m_pData;
}

//==============================================================================
inline
RenderStream  RenderStream::SubStream(int offset, int size) const
{
  XR_ASSERT(RenderStream, offset >= 0);
  XR_ASSERT(RenderStream, size >= 0);
  XR_ASSERT(RenderStream, offset + size <= m_capacity);
  return RenderStream(m_format, size, GetData(offset), false);
}

} // XR

#endif  //XR_RENDERSTREAM_HPP