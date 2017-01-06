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

namespace XR
{

//==============================================================================
///@brief Queues jobs and does them asynchronously.
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

  typedef XR::Queue<Job>  JobQueue;

  struct  Core
  {
    JobQueue    jobs;
    bool        isQuitting;

    void*       pStack;
    void*       pLockImpl;
    void*       pThreadImpl;
  };

  // structors
  explicit Worker(int stackSize);
  ~Worker();

  // general
  ///@return  Whether there are any jobs on the queue.
  bool  IsBusy() const;

  ///@brief Adds a job to the queue.
  void  Do(JobCallback pJobCb, void* pData); // no ownership transfer

  ///@brief  Removes all jobs from the queue.
  void  Abort();

private:
  // data
  Core  m_core;
};

} // XR

#endif  //XR_WORKER_HPP
