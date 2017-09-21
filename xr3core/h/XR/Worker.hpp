//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_WORKER_HPP
#define XR_WORKER_HPP

#include "Queue.hpp"
#include "Semaphore.hpp"
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
public:
  // types
  struct Job 
  {
    // structors
    virtual ~Job() {}

    // friends
    friend class Worker;

  protected:
    // virtual
    ///@brief The job may perform lazy initialisation, or reset its state
    /// if the instance is reused, in this method.
    virtual void Start() {}

    ///@brief Executes part (or whole) of the a job.
    ///@return Whether the job is complete. The Worker will call Process()
    /// repeatedly until it is.
    virtual bool Process() = 0;

    ///@brief Provides an opportunity to suspend the processing. The same
    /// Worker instance that is executing the Job is guaranteed to never
    /// call it concurrent to the execusion of Process().
    virtual void Suspend() {}

    ///@brief Provides an opportunity to resume the processing. The same
    /// Worker instance that is executing the Job is guaranteed to never
    /// call it concurrent to the execusion of Process().
    virtual void Resume() {}

    ///@brief Notifies the job of cancellation.
    virtual void Cancel() {}
  };

  // structors
  Worker();

  // general
  ///@brief Adds a job to the queue, if the thread has not yet been finalized.
  ///@return Whether the job has been added.
  bool  Enqueue(Job& job);  // no ownership transfer

  void  Suspend();
  void  Resume();

  ///@brief Removes all jobs whose processing has not started.
  void  CancelPendingJobs();

  ///@brief Finishes the processing of jobs.
  ///@note Must be called before reaching the destructor. Blocking call.
  void  Finalize();

private:
  // types
  typedef XR::Queue<Job*>  JobQueue;

  // data
  std::mutex              m_jobsMutex;
  Semaphore::Core         m_workSemaphore;
  JobQueue                m_jobs;

  bool                    m_finishing;
  std::thread             m_thread;

  std::mutex              m_suspendMutex;
  std::condition_variable m_suspendCV;
  bool                    m_isSuspended;

  // static
  static void ThreadFunction(Worker& worker);

  // internal
  void Loop();
};

} // XR

#endif  //XR_WORKER_HPP
