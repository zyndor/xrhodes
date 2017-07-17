//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Worker.hpp>
#include <XR/debug.hpp>
#include <XR/RefHolder.hpp>
#include <XR/ScopeGuard.hpp>

namespace XR
{

//==============================================================================
void Worker::ThreadFunction(Worker& worker)
{
  worker.Loop();
}

//==============================================================================
Worker::Worker()
: m_finalized(false),
  m_thread(ThreadFunction, MakeRefHolder(*this))
{}

//==============================================================================
void  Worker::Enqueue(Job j)
{
  XR_ASSERT(Worker, !m_finalized);
  XR_ASSERT(Worker, j.pJobCb != nullptr);
  {
    std::unique_lock<std::mutex> lock(m_jobsMutex);
    m_jobs.push_back(j);
  }
  m_workSemaphore.Post();
}

//==============================================================================
void Worker::CancelPendingJobs()
{
  std::unique_lock<std::mutex>  lock(m_jobsMutex);
  m_jobs.clear();
}

//==============================================================================
void  Worker::Finalize()
{
  m_finalized = true;
  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

//==============================================================================
void  Worker::Loop()
{
  while (!m_finalized)
  {
    Job j = { nullptr, nullptr };
    m_workSemaphore.Wait();

    {
      std::unique_lock<std::mutex>  lock(m_jobsMutex);
      j = m_jobs.front();
      m_jobs.pop_front();
    }

    (*j.pJobCb)(j.pData);
  }
}

} // XR
