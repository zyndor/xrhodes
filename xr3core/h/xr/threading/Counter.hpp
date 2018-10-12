#ifndef XR_COUNTER_HPP
#define XR_COUNTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Spinlock.hpp"
#include "xr/types/fundamentals.hpp"
#include "xr/debug.hpp"
#include <mutex>

namespace xr
{

//==============================================================================
struct SingleThreaded
{
  void Lock() {}
  void Unlock() {}
};

struct Spinlocked
{
public:
  void Lock()
  {
    m_lock.lock();
  }

  void Unlock()
  {
    m_lock.unlock();
  }

private:
  Spinlock m_lock;
};

struct Mutexed
{
  void Lock()
  {
    m_mutex.lock();
  }

  void Unlock()
  {
    m_mutex.unlock();
  }

private:
  std::mutex m_mutex;
};

//==============================================================================
///@brief Helps to tie the acquisition and release of resources to a scope, in
/// the vein of std::unique_lock<> and ScopeGuard.
template <class T, void(T::*Enter)(), void(T::*Exit)()>
class Frame
{
  XR_NONCOPY_DECL(Frame)

public:
  // structors
  Frame(T& obj)
  : m_frameObject(obj)
  {
    (obj.*Enter)();
  }

  ~Frame()
  {
    (m_frameObject.*Exit)();
  }

private:
  T& m_frameObject;
};

//==============================================================================
///@brief Facilitates counting of references, instances, locking objects, in
/// your threading environment of choice.
template <class ThreadingPolicy = SingleThreaded>
class Counter
{
public:
  using ThreadingScope = Frame<ThreadingPolicy, &ThreadingPolicy::Lock,
    &ThreadingPolicy::Unlock>;

  // structors
  ~Counter()
  {
    XR_ASSERTMSG(Counter, m_counter == 0, ("Destructor called with %d references leaked."));
  }

  // general
  void Acquire()
  {
    ThreadingScope lock(m_threading);
    ++m_counter;
  }

  void Release(bool& expired)
  {
    ThreadingScope lock(m_threading);
    XR_ASSERTMSG(Counter, m_counter > 0, ("Counter %p underrun.", this));
    expired = m_counter == 1;
    --m_counter;
  }

  void Release()
  {
    ThreadingScope lock(m_threading);
    XR_ASSERTMSG(Counter, m_counter > 0, ("Counter %p underrun.", this));
    --m_counter;
  }

  ///@return Whether the counter had been Acquire()d.
  bool IsEngaged() const
  {
    return GetCount() > 0;
  }

  int GetCount() const
  {
    ThreadingScope lock(m_threading);
    return m_counter;
  }

private:
  // data
  int m_counter = 0;
  mutable ThreadingPolicy m_threading;
};

} // xr

#endif //XR_COUNTER_HPP
