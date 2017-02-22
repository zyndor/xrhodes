//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_WORKER_HPP
#define XR_WORKER_HPP

#include "Queue.hpp"
#include "fundamentals.hpp"
#include <thread>
#include <mutex>
#include <atomic>

namespace XR
{

//==============================================================================
///@brief Worker runs a thread to perform Jobs on. The thread is only started
/// when jobs are enqueued, and once the queue of jobs have finished processing,
/// the Worker thread will check for new jobs a given number of times with a
/// given interval between each before it's terminated (Default is 100 times
/// with 10 milliseconds).
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
  explicit Worker();
  ~Worker();

  // general
  ///@brief Sets the number of retries and the interval between them
  /// for when the thread has no jobs.
  ///@note Changes will take place after the thread (re-)launch.
  void  SetIdleThreadExpiry(int numAttempts, int sleepIntervalMs);

  ///@brief Adds a job to the queue. If the thread wasn't running, this
  /// will start it.
  void  Enqueue(Job job);

  ///@brief Finishes the processing of jobs.
  ///@note Blocking call.
  void  Finalize();

private:
  // types
  typedef XR::Queue<Job>  JobQueue;

  // data
  mutable std::mutex  m_jobsMutex;
  JobQueue            m_jobs;

  std::thread         m_thread;

  int                 m_numAttempts;
  int                 m_sleepIntervalMs;

  // static
  static void ThreadFunction(Worker& worker);

  // internal
  void Loop();
};

} // XR

#endif  //XR_WORKER_HPP
