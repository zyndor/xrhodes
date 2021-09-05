#ifndef XR_BINARYHEAP_HPP
#define XR_BINARYHEAP_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <algorithm>

namespace xr
{
namespace detail
{
class BinaryHeapCore
{
protected:
  // data
  void* mData{};
  size_t mCapacity{};
  size_t mCount{};

  // structors
  BinaryHeapCore(void* data, size_t capacity, size_t count);
  ~BinaryHeapCore();

  // internal
  ///@return The number of items the storage allocated by the heap is sufficient
  /// to accommodate.
  size_t Capacity() const;

  ///@return The number of items on the heap.
  size_t Count() const;

  ///@return If there are no items on the heap.
  bool IsEmpty() const;
};

} // detail

//==============================================================================
///@brief Implements a binary heap with O(log N) insertion and removal, and O(1)
/// retrieval of the top priority element.
template <typename T, class Comp = std::less<T>>
class BinaryHeap: protected detail::BinaryHeapCore
{
public:
  // types
  using BinaryHeapCore::Capacity;
  using BinaryHeapCore::Count;
  using BinaryHeapCore::IsEmpty;

  // structors
  explicit BinaryHeap(size_t capacity = 0);

  BinaryHeap(BinaryHeap const& other) = delete;

  BinaryHeap(BinaryHeap&& other) noexcept;
  BinaryHeap& operator=(BinaryHeap other) noexcept;

  ~BinaryHeap();

  // general use
  ///@brief Reserves space for @a capacity items, only if @a capacity is greater
  /// than the number of items already on the heap.
  ///@note Will not reallocate / copy / alter capacity if requested capacity is
  /// less than what's required.
  void Reserve(size_t capacity);

  ///@brief Pushes @a item on the heap.
  void Push(T item);

  ///@brief Retrieves the top priority item.
  ///@note The heap must not be empty.
  T Peek() const;

  ///@brief Retrieves and removes the top priority item.
  ///@note The heap cannot be empty.
  T Pop();

  ///@brief Reduces the heap's storage to no more than what's necessary to
  /// accommodate its items.
  ///@note Will not reallocate / copy / alter capacity if already at minimum.
  void Shrink();

  ///@brief Removes all items and deletes the storage.
  void Destroy();

private:
  // internal
  void ReserveInternal(size_t capacity);
  void DestroyInternal(T* data);

  void Swim(size_t i);
  void Sink(size_t i);
};

//==============================================================================
template<typename T, class Comp>
BinaryHeap<T, Comp>::BinaryHeap(size_t capacity)
: BinaryHeapCore{ nullptr, capacity, 0 }
{
  Reserve(capacity);
}

//==============================================================================
template<typename T, class Comp>
BinaryHeap<T, Comp>::BinaryHeap(BinaryHeap&& other) noexcept
: BinaryHeapCore{ other.mData, other.mCapacity, other.mCount }
{
  other.mData = nullptr;
  other.mCapacity = 0;
  other.mCount = 0;
}

//==============================================================================
template<typename T, class Comp>
BinaryHeap<T, Comp>& BinaryHeap<T, Comp>::operator=(BinaryHeap other) noexcept
{
  std::swap(mData, other.mData);
  mCapacity = other.mCapacity;
  mCount = other.mCount;
  return *this;
}

//==============================================================================
template<typename T, class Comp>
BinaryHeap<T, Comp>::~BinaryHeap()
{
  Destroy();
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Reserve(size_t capacity)
{
  if (capacity > mCapacity)
  {
    ReserveInternal(capacity);
  }
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Push(T item)
{
  size_t i = mCount + 1;
  if (i > mCapacity)
  {
    Reserve(mCapacity * 2 + (mCapacity < 1));
  }

  static_cast<T*>(mData)[i] = item;
  mCount = i;
  Swim(i);
}

//==============================================================================
template<typename T, class Comp>
inline
T BinaryHeap<T, Comp>::Peek() const
{
  XR_ASSERT(BinaryHeap, mCount > 0);
  return static_cast<T*>(mData)[1];
}

//==============================================================================
template<typename T, class Comp>
T BinaryHeap<T, Comp>::Pop()
{
  auto data = static_cast<T*>(mData);
  T result = data[1];
  std::swap(data[1], data[mCount]);
  --mCount;
  Sink(1);
  data[mCount].~T();
  return result;
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Shrink()
{
  if (mCapacity > mCount)
  {
    ReserveInternal(mCount);
  }
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Destroy()
{
  DestroyInternal(nullptr);
  mCapacity = 0;
  mCount = 0;
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::ReserveInternal(size_t capacity)
{
  auto data = new T[capacity + 1];
  auto iCopy = static_cast<T*>(mData) + 1;
  std::copy(iCopy, iCopy + mCount, data + 1);

  DestroyInternal(data);
  mCapacity = capacity;
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::DestroyInternal(T* newData)
{
  delete[] static_cast<T*>(mData);
  mData = newData;
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Swim(size_t i)
{
  auto data = static_cast<T*>(mData);
  size_t iNext = i / 2;
  while (i > 1 && Comp()(data[iNext], data[i]))
  {
    std::swap(data[iNext], data[i]);
    i = iNext;
    iNext /= 2;
  }
}

//==============================================================================
template<typename T, class Comp>
void BinaryHeap<T, Comp>::Sink(size_t i)
{
  auto data = static_cast<T*>(mData);
  size_t iNext = i * 2;
  while (iNext <= mCount)
  {
    iNext += size_t(iNext < mCount && Comp()(data[iNext], data[iNext + 1]));
    if (!Comp()(data[i], data[iNext]))
    {
      break;
    }

    std::swap(data[i], data[iNext]);
    i = iNext;
    iNext *= 2;
  }
}

} // xr

#endif //XR_BINARYHEAP_HPP
