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
#include <XR/Semaphore.hpp>
#include <thread>
#include <mutex>
#include <atomic>

namespace XR
{

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
    JobCallback pExecuteCb;
    JobCallback pCancelCb;
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

  std::mutex  m_jobsMutex;
  Semaphore::Core m_workSemaphore;
  JobQueue    m_jobs;

  bool        m_finalized;
  std::thread m_thread;

  std::atomic<bool> m_isRunning;

  // static
  static void ThreadFunction(Worker& worker);

  // internal
  void Loop();
};

} // XR

#endif  //XR_WORKER_HPP
