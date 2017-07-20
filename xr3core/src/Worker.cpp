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
  XR_ASSERT(Worker, j.pExecuteCb != nullptr);
  if (!m_isRunning)
  {
    m_thread = std::thread(ThreadFunction, MakeRefHolder(*this));
    m_finalized = false;
  }

  {
    std::unique_lock<std::mutex> lock(m_jobsMutex);
    m_jobs.push_back(j);
    m_workSemaphore.Post();
  }
}

//==============================================================================
void Worker::CancelPendingJobs()
{
  JobQueue  q;
  {
    std::unique_lock<std::mutex>  lock(m_jobsMutex);
    q.adopt(m_jobs);

    // Consume posts. There should be as many posts as items cancelled.
    size_t posts = 0;
    while(m_workSemaphore.TryWait())
    {
      ++posts;
    }
    XR_ASSERT(Worker, posts == q.size());
  }

  std::for_each(q.begin(), q.end(), [](Job& j) {
    if (j.pCancelCb)
    {
      (*j.pCancelCb)(j.pData);
    }
  });
}

//==============================================================================
void  Worker::Finalize()
{
  {
    std::unique_lock<std::mutex>  lock(m_jobsMutex);
    m_finalized = true;
    m_workSemaphore.Post();
  }

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
    {
      std::unique_lock<std::mutex>  lock(m_jobsMutex);
      m_workSemaphore.Wait(lock);

      if (m_jobs.empty() && m_finalized)
      {
        break;
      }

      // Take the next job
      XR_ASSERT(Worker, !m_jobs.empty());
      j = m_jobs.front();
      m_jobs.pop_front();
    }

    (*j.pExecuteCb)(j.pData);
  }
}

} // XR
