//
// Nuclear Heart Games
// XRhodes
//
// HardList.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    09/03/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_HARDLIST_HPP
#define XR_HARDLIST_HPP

#include  "types.hpp"
#include  "typeutils.hpp"

namespace XR
{

//==============================================================================
class HardListCore
{
public:
  // structors
  HardListCore()
  : m_size(0),
    m_parBuffer(0)
  {}

  HardListCore(int capacity, void* parBuffer)
  : m_size(capacity),
    m_parBuffer(parBuffer)
  {}

  ~HardListCore()
  {}
  
  // general
  int   GetSize() const
  {
    return m_size;
  }

  void  SetSize(int n)
  {
    XR_ASSERT(HardListCore, n >= 0);
    m_size = n;
  }
    
protected:
  // data
  int   m_size;
  void* m_parBuffer; // no ownership
};

//==============================================================================
template  <typename T>
class HardList: protected HardListCore
{
public:
  // types
  typedef T ValueType;

  enum  { kIsLargeType = sizeof(ValueType) > 4 };

  typedef typename TypeSelect<kIsLargeType,
    bool(*)(const ValueType& v),
    bool(*)(ValueType v)>::Type IsActivePredicate;

  typedef typename TypeSelect<kIsLargeType,
    bool(*)(const ValueType& v, const ValueType& vMin),
    bool(*)(ValueType v, ValueType vMin)>::Type IsLeastActivePredicate;

  // using
  using HardListCore::GetSize;

  // structors
  HardList();
  explicit HardList(int capacity);
  ~HardList();

  // general
  T*        GetBuffer();
  const T*  GetBuffer() const;

  int GetActiveIndices(IsActivePredicate pIsActiveCb, int size,
    int* parIndices) const;

  void  SetSize(int n);

  int GetInsertionIndex(IsActivePredicate pIsActiveCb);

  int Insert(ValueType v, IsActivePredicate pIsActiveCb,
    IsLeastActivePredicate pIsLeastActiveCb);

  void  SetValuesTo(ValueType v);
  void  SetValuesTo(ValueType v, IsActivePredicate pIsActiveCb);

  void  Clear();

  // operator
  ValueType&        operator[](int i);
  const ValueType&  operator[](int i) const;
};

//==============================================================================
// implementation
//==============================================================================
template <class T>
HardList<T>::HardList()
: HardListCore()
{}

//==============================================================================
template <class T>
HardList<T>::HardList(int capacity)
: HardListCore(capacity, capacity > 0 ? new T[capacity] : 0)
{
  XR_ASSERT(HardList, capacity >= 0);
}

//==============================================================================
template <class T>
HardList<T>::~HardList()
{
  SetSize(0);
}

//==============================================================================
template <class T>
inline
T*  HardList<T>::GetBuffer()
{
  return static_cast<T*>(m_parBuffer);
}

//==============================================================================
template <class T>
inline
const T*  HardList<T>::GetBuffer() const
{
  return static_cast<T*>(m_parBuffer);
}

//==============================================================================
template <class T>
int HardList<T>::GetActiveIndices(IsActivePredicate pIsActiveCb, int size,
  int* parIndices) const
{
  XR_ASSERT(HardList, pIsActiveCb != 0);
  XR_ASSERT(HardList, parIndices != 0);
  int iInsert(0);
  for(int i = 0; i < m_size; ++i)
  {
    if((*pIsActiveCb)(static_cast<T*>(m_parBuffer)[i]))
    {
      XR_ASSERT(HardList, iInsert < size);
      parIndices[iInsert] = i;
      ++iInsert;
    }
  }

  return iInsert;
}

//==============================================================================
template <class T>
void  HardList<T>::SetSize(int n)
{
  XR_ASSERT(HardList, n >= 0);
  delete[] static_cast<T*>(m_parBuffer);

  HardListCore::SetSize(n);
  m_parBuffer = n > 0 ? new ValueType[n] : 0;
}

//==============================================================================
template <class T>
int HardList<T>::Insert(ValueType v, IsActivePredicate pIsActiveCb)
{
  XR_ASSERT(HardList, pIsActiveCb != 0);
  int i(GetInsertionIndex(pIsActiveCb));
  XR_ASSERT(HardList, i < m_size);
  static_cast<T*>(m_parBuffer)[i] = v;
  return i;
}

//==============================================================================
template <class T>
int HardList<T>::GetInsertionIndex(IsActivePredicate pIsActiveCb)
{
  XR_ASSERT(HardList, pIsActiveCb != 0);
  int i(0);
  while(i < m_size)
  {
    ValueType&  vi(static_cast<T*>(m_parBuffer)[i]);
    if(!(*pIsActiveCb)(vi))
    {
      break;
    }
    ++i;
  }
  return i;
}

//==============================================================================
template <class T>
int HardList<T>::Insert(ValueType v, IsActivePredicate pIsActiveCb,
  IsLeastActivePredicate pIsLeastActiveCb)
{
  XR_ASSERT(HardList, pIsActiveCb != 0);
  XR_ASSERT(HardList, pIsLeastActiveCb != 0);

  int       iMin(m_size);
  int       i(0);
  ValueType vMin(v);
  while(i < m_size)
  {
    ValueType&  vi(static_cast<T*>(m_parBuffer)[i]);
    if((*pIsActiveCb)(vi))
    {
      if((*pIsLeastActiveCb)(vi, vMin))
      {
        vMin = vi;
        iMin = i;
      }
    }
    else
    {
      vi = v;
      break;
    }
    ++i;
  }

  if(i == m_size && iMin < m_size)
  {
    static_cast<T*>(m_parBuffer)[iMin] = v;
    i = iMin;
  }
  return i;
}

//==============================================================================
template <class T>
void  HardList<T>::SetValuesTo(ValueType v)
{
  for(int i = 0; i < m_size; ++i)
  {
    static_cast<T*>(m_parBuffer)[i] = v;
  }
}

//==============================================================================
template <class T>
void  HardList<T>::SetValuesTo(ValueType v, IsActivePredicate pIsActiveCb)
{
  XR_ASSERT(HardList, pIsActiveCb != 0);
  for(int i = 0; i < m_size; ++i)
  {
    ValueType&  vi(static_cast<T*>(m_parBuffer)[i]);
    if((*pIsActiveCb)(vi))
    {
      vi = v;
    }
  }
}

//==============================================================================
template <class T>
void  HardList<T>::Clear()
{
  memset(m_parBuffer, 0x00, sizeof(T) * m_size);
}

//==============================================================================
template <class T>
T&  HardList<T>::operator[](int i)
{
  XR_ASSERT(HardList, m_parBuffer != 0);
  XR_ASSERT(HardList, i >= 0);
  XR_ASSERT(HardList, i < m_size);

  return static_cast<T*>(m_parBuffer)[i];
}

//==============================================================================
template <class T>
const T&  HardList<T>::operator[](int i) const
{
  XR_ASSERT(HardList, m_parBuffer != 0);
  XR_ASSERT(HardList, i >= 0);
  XR_ASSERT(HardList, i < m_size);

  return static_cast<T*>(m_parBuffer)[i];
}

} // XR

#endif  // XR_HARDLIST_HPP