//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SEMAPHORE_HPP
#define XR_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

namespace XR
{

// Simple semaphore implementation based on threading primitives available in C++11.
class Semaphore
{
public:
  // general
  void Post()
  {
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    ++m_counter;
    m_cv.notify_one();
  }

  void Wait()
  {
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    while (!m_counter) // Handle spurious wake-ups.
    {
      m_cv.wait(lock);
    }
    --m_counter;
  }

  bool TryWait()
  {
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    if (m_counter)
    {
      --m_counter;
      return true;
    }
    return false;
  }

private:
  // data
  std::mutex m_mutex;
  std::condition_variable m_cv;
  uint32_t m_counter = 0; // Initialized as locked.
};

}

#endif  //XR_SEMAPHORE_HPP
