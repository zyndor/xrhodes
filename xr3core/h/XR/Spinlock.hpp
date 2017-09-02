//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SPINLOCK_HPP
#define XR_SPINLOCK_HPP

#include <atomic>

namespace XR
{

//==============================================================================
///@brief Synchronisation primitive for that provides non-recursive mutual
/// exclusion between threads of the same process. Satisfies the Lockable
/// concept.
struct Spinlock
{
  // general
  ///@brief Busy waits for the first opportunity to lock.
  /// Lets flow through once it has locked.
  void lock()
  {
    while (m_lock.test_and_set(std::memory_order_acquire))
    {}
  }

  ///@brief Attempts to lock.
  ///@return Whether it was able to.
  bool try_lock()
  {
    bool result = !m_lock.test_and_set(std::memory_order_acquire);
    return result;
  }

  ///@brief Releases the lock.
  void unlock()
  {
    m_lock.clear(std::memory_order_release);
  }

private:
  // data
  std::atomic_flag  m_lock = ATOMIC_FLAG_INIT;
};

} // XR

#endif // XR_SPINLOCK_HPP
