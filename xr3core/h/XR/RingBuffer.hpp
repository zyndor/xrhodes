#ifndef XR_RINGBUFFER_HPP
#define XR_RINGBUFFER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "debug.hpp"
#include <vector>
#include <cstdint>

namespace XR
{

//==============================================================================
class RingBufferCore
{
protected:
  // data
  size_t  m_front;
  size_t  m_back;
  size_t  m_elems;

  // structors
  RingBufferCore();
  ~RingBufferCore();

  // general
  size_t  GetNumElements() const;
  bool    HasElements() const;

  void    Clear();
};

//==============================================================================
///@brief Vector-based RingBuffer implementation that allows bidirectional
/// access, i.e. putting to and getting from both ends.
template  <typename Type>
class RingBuffer: protected RingBufferCore
{
public:
  // types
  typedef Type                  ValueType;
  typedef RingBuffer<ValueType> SelfType;
  
  // using
  using RingBufferCore::GetNumElements;
  using RingBufferCore::HasElements;
  
  // structors
  RingBuffer();
  explicit RingBuffer(size_t size);
  ~RingBuffer();

  // general
  size_t            Size() const;
  void              Resize(size_t size);
  
  void              PutBack(ValueType d);
  void              PutFront(ValueType d);

  ValueType&        GetBack();
  const ValueType&  GetBack() const;

  ValueType&        GetFront();
  const ValueType&  GetFront() const;

  ValueType&        Get(size_t id);
  const ValueType&  Get(size_t id) const;
  
  void        PopBack();
  void        PopFront();

  void        Clear();

  ValueType&  operator[](size_t id);
  const ValueType&  operator[](size_t id) const;

private:
  // types
  typedef std::vector<ValueType> ValueVector;
  
  // internal
  // data
  ValueVector m_data;
};

//==============================================================================
// implementation 
//==============================================================================
///@return  The number of elements actually added to the RingBuffer.
inline
size_t  RingBufferCore::GetNumElements() const
{
  return m_elems;
}

//==============================================================================
///@return  Whether there are any elements in the RingBuffer.
inline
bool  RingBufferCore::HasElements() const
{
  return m_elems > 0;
}

//==============================================================================
///@brief Reset the element count. Does not actually destroy elements.
inline
void  RingBufferCore::Clear()
{
  m_elems = 0;
  m_front = 0;
  m_back = 0;
}

//==============================================================================
template  <typename Type>
RingBuffer<Type>::RingBuffer()
: RingBufferCore(),
  m_data()
{}

//==============================================================================
template  <typename Type>
RingBuffer<Type>::RingBuffer(size_t size)
: RingBufferCore(),
  m_data(size)
{}

//==============================================================================
template  <typename Type>
RingBuffer<Type>::~RingBuffer()
{}

//==============================================================================
///@return  The size of the underlying array.
template  <typename Type>
inline
size_t  RingBuffer<Type>::Size() const
{
  return m_data.size();
}

//==============================================================================
///@brief Changes the size of the underlying array. The contents of the
/// RingBuffer are invalidated.
template  <typename Type>
void  RingBuffer<Type>::Resize(size_t size)
{
  m_data.resize(size);
  Clear();
}

//==============================================================================
///@brief Puts an element @a d to after the last element in the buffer. If the
/// number of elements before insertion equal the capacity of the underlying
/// vector, then this new element will overwrite the first element in the
/// buffer.
template  <typename Type>
void  RingBuffer<Type>::PutBack(ValueType d)
{
  m_data[m_back] = d;

  const size_t  size(Size());
  ++m_back;
  if (m_back == size)
  {
    m_back = 0;
  }

  if (m_elems < size)
  {
    ++m_elems;
  }
  else
  {
    m_front = m_back;
  }
}

//==============================================================================
///@brief Puts an element @a d to before the first element in the buffer. If the
/// number of elements before insertion equal the capacity of the underlying
/// vector, then this new element will overwrite the last element in the buffer.
template  <typename Type>
void  RingBuffer<Type>::PutFront(ValueType d)
{
  const size_t  size(Size());
  if (m_front == 0)
  {
    m_front = size;
  }
  --m_front;

  m_data[m_front] = d;

  if (m_elems < size)
  {
    ++m_elems;
  }
  else
  {
    m_back = m_front;
  }
}

//==============================================================================
///@brief Removes and returns the last element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
Type&  RingBuffer<Type>::GetBack()
{
  XR_ASSERT(RingBuffer, HasElements());
  size_t  back(m_back);
  if (back == 0)
  {
    back = Size();
  }
  --back;
  return m_data[back];
}

//==============================================================================
///@brief Removes and returns the last element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
const Type&  RingBuffer<Type>::GetBack() const
{
  XR_ASSERT(RingBuffer, HasElements());
  size_t  back(m_back);
  if (back == 0)
  {
    back = Size();
  }
  --back;
  return m_data[back];
}

//==============================================================================
///@brief Removes and returns the first element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
Type&  RingBuffer<Type>::GetFront()
{
  XR_ASSERT(RingBuffer, HasElements());
  return m_data[m_front];
}

//==============================================================================
///@brief Removes and returns the first element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
const Type&  RingBuffer<Type>::GetFront() const
{
  XR_ASSERT(RingBuffer, HasElements());
  return m_data[m_front];
}

//==============================================================================
///@brief Removes the last element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
void  RingBuffer<Type>::PopBack()
{
  XR_ASSERT(RingBuffer, HasElements());

  --m_elems;
  if (m_back == 0)
  {
    m_back = Size();
  }
  --m_back;
}

//==============================================================================
///@brief Removes the first element from the buffer.
///@note  You will have to make sure that there are elements in the buffer to
/// begin with.
template  <typename Type>
void  RingBuffer<Type>::PopFront()
{
  XR_ASSERT(RingBuffer, HasElements());

  --m_elems;
  ++m_front;
  if (m_front == Size())
  {
    m_front = 0;
  }
}

//==============================================================================
///@brief Clears the buffer.
template  <typename Type>
void  RingBuffer<Type>::Clear()
{
  RingBufferCore::Clear();

  const size_t  size(Size());
  if (m_elems < size)
  {
    for (size_t i = m_front; i < m_back; ++i)
    {
      m_data[i] = Type();
    }
  }
  else
  {
    for (size_t i = m_front; i < size; ++i)
    {
      m_data[i] = Type();
    }

    for (size_t i = 0; i < m_back; ++i)
    {
      m_data[i] = Type();
    }
  }
}

//==============================================================================
///@brief Get nth element from start of the buffer
template  <typename Type>
Type&  RingBuffer<Type>::Get(size_t id)
{
  XR_ASSERT(RingBuffer, id >= 0);
  XR_ASSERT(RingBuffer, id < m_elems);
  return m_data[(m_front + id) % Size()];
}

//==============================================================================
///@brief Get nth element from start of the buffer
template  <typename Type>
const Type&  RingBuffer<Type>::Get(size_t id) const
{
  XR_ASSERT(RingBuffer, id >= 0);
  XR_ASSERT(RingBuffer, id < m_elems);
  return m_data[(m_front + id) % Size()];
}

//==============================================================================
///@brief Get nth element from the underlying array.
template  <typename Type>
Type&  RingBuffer<Type>::operator[](size_t id)
{
  XR_ASSERT(RingBuffer, id >= 0);
  XR_ASSERT(RingBuffer, id < m_data.size());
  return m_data[id];
}

//==============================================================================
///@brief Get nth element from the underlying array.
template  <typename Type>
const Type&  RingBuffer<Type>::operator[](size_t id) const
{
  XR_ASSERT(RingBuffer, id >= 0);
  XR_ASSERT(RingBuffer, id < m_data.size());
  return m_data[id];
}

} // XR

#endif //XR_RINGBUFFER_HPP
