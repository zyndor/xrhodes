#ifndef XR_SHAREDPOOLALLOCATOR_HPP
#define XR_SHAREDPOOLALLOCATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <stddef.h>
#include "Pool.hpp"

namespace XR
{

//==============================================================================
class SharedPoolAllocatorCore
{
public:
  explicit SharedPoolAllocatorCore(Pool *pPool) throw();
  ~SharedPoolAllocatorCore() throw();
  
protected:
  Pool *m_pPool;
};

//==============================================================================
///@brief The SharedPoolAllocator template class is an allocator that is useful
/// in single frame allocation environments.
template  <typename Type> class SharedPoolAllocator;

template  <>
class SharedPoolAllocator<void>
{
public:
  // types
  typedef void value_type;
  
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;

  template <typename OtherType>
  struct rebind
  {
    typedef SharedPoolAllocator<OtherType> other;
  };
  
  // friends
  template <class OtherType> friend class SharedPoolAllocator;
};

template  <typename Type>
class SharedPoolAllocator:  protected SharedPoolAllocatorCore
{
public:
  // types
  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  
  typedef Type value_type;
  
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  
  typedef value_type&       reference;
  typedef const value_type& const_reference;

  template <typename OtherType>
  struct rebind
  {
    typedef SharedPoolAllocator<OtherType> other;
  };
  
  // structors
  explicit SharedPoolAllocator(Pool &mp) throw();
  SharedPoolAllocator(const SharedPoolAllocator &rhs) throw();
  
  template <typename OtherType>
  SharedPoolAllocator(const SharedPoolAllocator<OtherType> &rhs) throw();
  
  ~SharedPoolAllocator() throw();

  // general use
  pointer address(reference x) const;
  const_pointer address(const_reference x) const;

  pointer allocate(size_type n, 
    SharedPoolAllocator<void>::const_pointer hint = 0);
  
  void deallocate(pointer p, size_type n);
  
  size_type max_size() const throw();

  void construct(pointer p, const value_type& val);
  void destroy(pointer p);
  
  // friends
  template <class OtherType> friend class SharedPoolAllocator;
};

//==============================================================================
template <class Type, class OtherType>
bool operator==(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) throw();

template <class Type, class OtherType>
bool operator!=(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) throw();

//==============================================================================
// implementation
//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::SharedPoolAllocator(Pool& mp) throw()
: SharedPoolAllocatorCore(&mp)
{}

//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::
  SharedPoolAllocator(const SharedPoolAllocator& rhs) throw()
: SharedPoolAllocatorCore(rhs.m_pPool)
{}

//==============================================================================
template  <typename Type>
template  <typename OtherType>
SharedPoolAllocator<Type>::
  SharedPoolAllocator(const SharedPoolAllocator<OtherType>& rhs) throw()
: SharedPoolAllocatorCore(rhs.m_pPool)
{}

//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::~SharedPoolAllocator() throw()
{}

//==============================================================================
template  <typename Type>
inline
typename SharedPoolAllocator<Type>::pointer
  SharedPoolAllocator<Type>::address(reference x) const
{
  return &x;
}

//==============================================================================
template  <typename Type>
inline
typename SharedPoolAllocator<Type>::const_pointer
  SharedPoolAllocator<Type>::address(const_reference x) const
{
  return &x;
}

//==============================================================================
template  <typename Type>
inline
typename SharedPoolAllocator<Type>::pointer
  SharedPoolAllocator<Type>::allocate(size_type n,
    SharedPoolAllocator<void>::const_pointer hint)
{
  return static_cast<pointer>(m_pPool->Allocate(n * sizeof(value_type)));
}

//==============================================================================
template  <typename Type>
inline
void SharedPoolAllocator<Type>::deallocate(pointer p, size_type n)
{
  // do the nothing
}

//==============================================================================
template  <typename Type>
inline
typename SharedPoolAllocator<Type>::size_type
  SharedPoolAllocator<Type>::max_size() const throw()
{
  return m_pPool->CalculateFree();
}

//==============================================================================
template  <typename Type>
inline
void SharedPoolAllocator<Type>::construct(pointer p, const value_type& val)
{
  new (p) Type(val);
}

//==============================================================================
template  <typename Type>
inline
void SharedPoolAllocator<Type>::destroy(pointer p)
{
  p->~Type();
}

//==============================================================================
template <class Type, class OtherType>
bool operator==(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) throw()
{
  return true;
}

//==============================================================================
template <class Type, class OtherType>
bool operator!=(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) throw()
{
  return true;
}

} // XR

#endif //XR_SHAREDPOOLALLOCATOR_HPP
