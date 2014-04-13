//
// Nuclear Heart Games
// XRhodes
//
// HardStack.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    29/11/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_STACK_HPP
#define XR_STACK_HPP

#include  "types.hpp"

namespace XR
{

//==============================================================================
class HardStackCore
{
public:
  enum { kEmptyStackSize = -1 };

  HardStackCore()
  : m_id(kEmptyStackSize)
  {}

  int m_id;
};

//==============================================================================
template  <class T, uint32 kMax>
class HardStack:  protected HardStackCore
{
public:
  // types
  typedef T Type;

  enum  { kMaxSize = kMax };

  // using
  using HardStackCore::kEmptyStackSize;

  // structors
  HardStack()
  : HardStackCore()
  //, m_arData()  // VC++ <3s you
  {}

  ~HardStack()
  {}

  // general use
  bool  IsEmpty() const
  {
    return m_id == kEmptyStackSize;
  }

  int NumElements() const
  {
    return m_id + 1;
  }

  Type& Top()
  {
    XR_ASSERT(HardStack, m_id > kEmptyStackSize);
    return m_arData[m_id];
  }

  const Type& Top() const
  {
    XR_ASSERT(HardStack, m_id > kEmptyStackSize);
    return m_arData[m_id];
  }

  void  Push(const Type& elem)
  {
    ++m_id;
    XR_ASSERT(HardStack, m_id < kMaxSize);
    m_arData[m_id] = elem;
  }

  void  Pop()
  {
    XR_ASSERT(HardStack, m_id > kEmptyStackSize);
    m_arData[m_id].~Type();
    --m_id;
  }

  void  Reset()
  {
    m_id = kEmptyStackSize;
  }

private:
  // data
  Type  m_arData[kMaxSize];
};

} // XR

#endif // XR_STACK_HPP
