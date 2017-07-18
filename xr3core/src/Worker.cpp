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
  auto& isRunning = worker.m_isRunning;
  isRunning = true;
  auto guard = MakeScopeGuard([&isRunning] { isRunning = false; });

  worker.Loop();
}

//==============================================================================
Worker::Worker()
: m_finalized(false),
  m_isRunning(false),
  m_thread()
{}

//==============================================================================
void  Worker::Enqueue(Job j)
{
  XR_ASSERT(Worker, j.pJobCb != nullptr);
  if (!m_isRunning)
  {
    m_thread = std::thread(ThreadFunction, MakeRefHolder(*this));
    m_finalized = false;
  }

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
  m_workSemaphore.Post();

  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

//==============================================================================
void  Worker::Loop()
{
  Job j;
  while (true)
  {
    m_workSemaphore.Wait();

    {
      std::unique_lock<std::mutex>  lock(m_jobsMutex);
      if (m_jobs.empty())
      {
        if (m_finalized)
        {
          break;
        }
        else
        {
          continue;
          // TODO: some more elegant solution would be great;
        }
      }

      // Take next job
      XR_ASSERT(Worker, !m_jobs.empty());
      j = m_jobs.front();
      m_jobs.pop_front();
    }

    (*j.pJobCb)(j.pData);
  }
}

} // XR
