#ifndef XR_OBJECTPOOL_HPP
#define XR_OBJECTPOOL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/IndexServer.hpp"
#include "xr/memory/memory.hpp"
#include "xr/threading/Counter.hpp"
#include <memory>

namespace xr
{
namespace detail
{

//==============================================================================
class ObjectPoolCore
{
  XR_NONCOPY_DECL(ObjectPoolCore)

public:
  // data
  std::byte* mBuffer;
  xr::IndexServer mHandles;

  // structors
  explicit ObjectPoolCore(IndexServer::Index numHandles, size_t objectSize) noexcept;
  ~ObjectPoolCore() noexcept;
};

}

//==============================================================================
///@brief Preallocates memory, and serves it up on demand, in an efficient manner,
/// as object of type T.
template <class T, class ThreadingPolicy = SingleThreaded>
class ObjectPool
{
  XR_NONOBJECT_DECL(ObjectPool)

public:
  // types
  using ThreadingScope = Frame<ThreadingPolicy, &ThreadingPolicy::Lock,
    &ThreadingPolicy::Unlock>;

  ///@brief Inheriting from this class overrides calls to new and delete, to source
  /// the memory from an ObjectPool of the derived type.
  ///@note The XR_OBJECTPOOL_CLIENT() macro is at your disposal for declaring your
  /// derived classes.
  struct Client
  {
    void* operator new(size_t count) noexcept
    {
      return ObjectPool<T>::Acquire(count);
    }

    void operator delete(void* p) noexcept
    {
      ObjectPool<T>::Release(p);
    }

  protected:
    ~Client() = default;
  };

  // static
  ///@brief Initializes the pool (which must not have been already initialised),
  /// for @a numHandles objects.
  static void Init(IndexServer::Index numHandles)
  {
    ThreadingScope lock(sThreading);
    XR_ASSERT(ObjectPool, !sCore);
    sCore.reset(new detail::ObjectPoolCore(numHandles, sizeof(T)));
  }

  ///@brief Terminates the pool, releasing its memory. All handles should be Release()d
  /// prior to this; the result of subsequent Acquire()s is undefined.
  static void Shutdown() noexcept
  {
    ThreadingScope lock(sThreading);
    XR_TRACEIF(ObjectPool, sCore->mHandles.GetNumAcquired() > 0,
      ("%s: %d handles leaked", __FUNCTION__, sCore->mHandles.GetNumAcquired()));
    sCore.reset();
  }

private:
  // static
  static ThreadingPolicy sThreading;
  static std::unique_ptr<detail::ObjectPoolCore> sCore;

  ///@brief Acquires raw memory from the ObjectPool.
  ///@return Pointer to the chunk, or nullptr if we've run out of handles.
  static void* Acquire([[maybe_unused]] size_t count) noexcept
  {
    XR_ASSERT(ObjectPool, count == sizeof(T));
    ThreadingScope lock(sThreading);
    size_t handle = sCore->mHandles.Acquire();
    void* mem = handle != IndexServer::kInvalidIndex ?
      sCore->mBuffer + handle * sizeof(T) : nullptr;
    return mem;
  }

  ///@brief Releases the memory back to the ObjectPool, making it available for
  /// a future Acquire().
  static void Release(void* object) noexcept
  {
    ThreadingScope lock(sThreading);
    auto bytes = static_cast<std::byte*>(object);
    size_t handle = bytes - sCore->mBuffer;
    XR_ASSERT(ObjectPool, handle % sizeof(T) == 0);
    XR_ASSERT(ObjectPool, handle / sizeof(T) >= 0);
    XR_DEBUG_ONLY(std::fill(bytes, bytes + sizeof(T), 0xdd));
    sCore->mHandles.Release(IndexServer::Index(handle / sizeof(T)));
  }
};

template <class T, class ThreadingPolicy>
ThreadingPolicy ObjectPool<T, ThreadingPolicy>::sThreading;

template <class T, class ThreadingPolicy>
std::unique_ptr<detail::ObjectPoolCore> ObjectPool<T, ThreadingPolicy>::sCore;

} // xr

///@brief Facilitates the declaration of classes that rely on xr::ObjectPool for
/// memory allocation.
#define XR_OBJECTPOOL_CLIENT(x) public xr::ObjectPool<x>::Client

#endif //XR_OBJECTPOOL_HPP
