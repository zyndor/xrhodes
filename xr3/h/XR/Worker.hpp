//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_WORKER_HPP
#define XR_WORKER_HPP

#include <XR/Queue.hpp>
#include <XR/fundamentals.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace XR
{

class Semaphore
{
public:
  void Post() {
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    ++m_counter;
    m_cv.notify_one();
  }

  void Wait() {
    std::unique_lock<decltype(m_mutex)> lock(m_mutex);
    while (!m_counter)
    {
      // Handle spurious wake-ups.
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

//==============================================================================
///@brief Worker is a generic producer / consumer abstraction; it runs a thread
/// to perform Jobs on.
///@note The user must make sure that all jobs have finished processing before
/// the execution reaches the Worker destructor. Finalize() helps facilitate
/// this.
class Worker
{
  XR_NONCOPY_DECL(Worker)

public:
  // types
  typedef void (*JobCallback)(void*);

  struct Job 
  {
    JobCallback pJobCb;
    void*       pData;
  };

  // structors
  Worker();

  // general
  ///@brief Adds a job to the queue. If the thread wasn't running, this
  /// will start it.
  void  Enqueue(Job job);

  ///@brief Removes all jobs that have not been started processing.
  void  CancelPendingJobs();

  ///@brief Finishes the processing of jobs.
  ///@note Blocking call.
  void  Finalize();

private:
  // types
  typedef XR::Queue<Job>  JobQueue;

  // data
  Semaphore   m_workSemaphore;

  std::mutex  m_jobsMutex;
  JobQueue    m_jobs;

  bool        m_finalized;
  std::thread m_thread;

  // static
  static void ThreadFunction(Worker& worker);

  // internal
  void Loop();
};

} // XR

#endif  //XR_WORKER_HPP
