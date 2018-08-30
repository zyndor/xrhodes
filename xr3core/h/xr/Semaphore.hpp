#ifndef XR_SEMAPHORE_HPP
#define XR_SEMAPHORE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <mutex>
#include <condition_variable>

namespace xr
{

//=============================================================================
///@brief Simple semaphore implementation based on threading primitives
/// available in C++11.
class Semaphore
{
public:
  // types
  using Mutex = std::mutex;
  using UniqueLock = std::unique_lock<Mutex>;

  ///@brief Enables client code to synchronize to its pre-defined mutex.
  class Core
  {
  public:
    void Post()
    {
      ++m_counter;
      m_cv.notify_one();
    }

    void Wait(UniqueLock& lock)
    {
      while (m_counter == 0) // Handle spurious wake-ups.
      {
        m_cv.wait(lock);
      }
      --m_counter;
    }

    bool TryWait()
    {
      if (m_counter > 0)
      {
        --m_counter;
        return true;
      }
      else
      {
        return false;
      }
    }

  private:
    // data
    std::condition_variable m_cv;
    uint32_t m_counter = 0; // Initialized as locked.
  };


  // general
  void Post()
  {
    UniqueLock lock(m_mutex);
    m_core.Post();
  }

  void Wait()
  {
    UniqueLock lock(m_mutex);
    m_core.Wait(lock);
  }

  bool TryWait()
  {
    UniqueLock lock(m_mutex);
    return m_core.TryWait();
  }

private:
  // data
  Mutex m_mutex;
  Core m_core;
};

}

#endif  //XR_SEMAPHORE_HPP
