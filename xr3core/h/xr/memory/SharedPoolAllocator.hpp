#ifndef XR_SHAREDPOOLALLOCATOR_HPP
#define XR_SHAREDPOOLALLOCATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <stddef.h>
#include "Pool.hpp"

namespace xr
{

//==============================================================================
class SharedPoolAllocatorCore
{
public:
  explicit SharedPoolAllocatorCore(Pool* pool) noexcept;
  ~SharedPoolAllocatorCore() noexcept;

protected:
  Pool *m_pool;
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
  explicit SharedPoolAllocator(Pool &mp) noexcept;
  SharedPoolAllocator(const SharedPoolAllocator &rhs) noexcept;

  template <typename OtherType>
  SharedPoolAllocator(const SharedPoolAllocator<OtherType> &rhs) noexcept;

  ~SharedPoolAllocator() noexcept;

  // general use
  pointer address(reference x) const;
  const_pointer address(const_reference x) const;

  pointer allocate(size_type n,
    SharedPoolAllocator<void>::const_pointer hint = 0);

  void deallocate(pointer p, size_type n);

  size_type max_size() const noexcept;

  void construct(pointer p, const value_type& val);
  void destroy(pointer p);

  // friends
  template <class OtherType> friend class SharedPoolAllocator;
};

//==============================================================================
template <class Type, class OtherType>
constexpr bool operator==(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) noexcept;

template <class Type, class OtherType>
constexpr bool operator!=(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) noexcept;

//==============================================================================
// implementation
//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::SharedPoolAllocator(Pool& mp) noexcept
: SharedPoolAllocatorCore(&mp)
{}

//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::
  SharedPoolAllocator(const SharedPoolAllocator& rhs) noexcept
: SharedPoolAllocatorCore(rhs.m_pool)
{}

//==============================================================================
template  <typename Type>
template  <typename OtherType>
SharedPoolAllocator<Type>::
  SharedPoolAllocator(const SharedPoolAllocator<OtherType>& rhs) noexcept
: SharedPoolAllocatorCore(rhs.m_pool)
{}

//==============================================================================
template  <typename Type>
SharedPoolAllocator<Type>::~SharedPoolAllocator() noexcept
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
    SharedPoolAllocator<void>::const_pointer /*hint*/)
{
  return static_cast<pointer>(m_pool->Allocate(n * sizeof(value_type)));
}

//==============================================================================
template  <typename Type>
inline
void SharedPoolAllocator<Type>::deallocate(pointer /*p*/, size_type /*n*/)
{
  // do the nothing
}

//==============================================================================
template  <typename Type>
inline
typename SharedPoolAllocator<Type>::size_type
  SharedPoolAllocator<Type>::max_size() const noexcept
{
  return m_pool->CalculateFree();
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
constexpr bool operator==(const SharedPoolAllocator<Type>&,
  const SharedPoolAllocator<OtherType>&) noexcept
{
  return true;  // de-allocation is no-op, so sure, go ahead.
}

//==============================================================================
template <class Type, class OtherType>
constexpr bool operator!=(const SharedPoolAllocator<Type>& lhs,
  const SharedPoolAllocator<OtherType>& rhs) noexcept
{
  return !(lhs == rhs);
}

} // xr

#endif //XR_SHAREDPOOLALLOCATOR_HPP
