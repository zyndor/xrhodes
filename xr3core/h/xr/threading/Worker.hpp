#ifndef XR_WORKER_HPP
#define XR_WORKER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Semaphore.hpp"
#include "xr/memory/Queue.hpp"
#include <thread>
#include <mutex>
#include <atomic>

namespace xr
{

//==============================================================================
///@brief Worker is a generic producer / consumer abstraction; it runs a thread
/// to perform Jobs on.
///@note The user must call Finalize() before the execution reaches the Worker
/// destructor.
class Worker
{
public:
  // types
  struct Job
  {
    // structors
    virtual ~Job() {}

    // virtual
    ///@brief The job may perform lazy initialisation, or reset its state
    /// if the instance is reused, in this method.
    virtual void Start() {}

    ///@brief Executes part (or whole) of the a job.
    ///@return Whether the job is complete. The Worker will call Process()
    /// repeatedly until it returns true.
    virtual bool Process() = 0;

    ///@brief Provides an opportunity to suspend the processing. The same
    /// Worker instance that is executing the Job is guaranteed to never
    /// call it concurrent to the execusion of Process().
    virtual void Suspend() {}

    ///@brief Provides an opportunity to resume the processing. The same
    /// Worker instance that is executing the Job is guaranteed to never
    /// call it concurrent to the execusion of Process().
    virtual void Resume() {}

    ///@brief Notifies the job of cancellation. Currently this may only
    /// happen instead of starting and processing - jobs that have
    /// started processing can be suspended but not cancelled.
    virtual void Cancel() {}
  };

  // structors
  Worker();

  // general
  ///@brief Adds a job to the queue, only if the worker thread is still running
  /// (it may have been finalized). If the thread is no longer running, this
  /// will Cancel() @a job.
  ///@note Enqueuing Jobs from the worker thread is allowed, but it carries the
  /// risk of never finishing.
  void  Enqueue(Job& job);  // no ownership transfer

  void  Suspend();
  void  Resume();

  ///@brief Removes all jobs whose processing has not started.
  void  CancelPendingJobs();

  ///@brief Finishes the processing of jobs synchronously.
  ///@note Must be called before reaching the destructor.
  ///@note Obviously it must not be called from the worker thread.
  void  Finalize();

private:
  // types
  using JobQueue = Queue<Job*>;

  // data
  std::mutex              m_jobsMutex;
  Semaphore::Core         m_workSemaphore;
  JobQueue                m_jobs; // no ownership

  bool                    m_finishing;
  std::thread             m_thread;

  std::mutex              m_suspendMutex;
  std::condition_variable m_suspendCV;
  bool                    m_isSuspended;

  // internal
  void Loop();
};

} // xr

#endif  //XR_WORKER_HPP
