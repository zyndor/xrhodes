//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Worker.hpp>
#include <XR/debug.hpp>
#include <XR/RefHolder.hpp>

namespace XR
{

//==============================================================================
void Worker::ThreadFunction(Worker& worker)
{
  worker.Loop();
}

//==============================================================================
Worker::Worker()
: m_finishing(false),
  m_thread(std::thread(ThreadFunction, MakeRefHolder(*this)))
{}

//==============================================================================
bool  Worker::Enqueue(Job& j)
{
  std::unique_lock<std::mutex> lock(m_jobsMutex);
  bool result = !m_finishing;
  if (result)
  {
    m_jobs.push_back(&j);
    m_workSemaphore.Post();
  }
  return result;
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

  std::for_each(q.begin(), q.end(), [](Job* j) {
    j->Cancel();
  });
}

//==============================================================================
void  Worker::Finalize()
{
  {
    std::unique_lock<std::mutex>  lock(m_jobsMutex);
    if (!m_finishing)
    {
      m_finishing = true;
      m_workSemaphore.Post();
    }
    else
    {
      XR_TRACE(Worker, ("Worker %p already finalized.", this));
    }
  }

  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

//==============================================================================
void  Worker::Loop()
{
  Job* job;
  while (true)
  {
    {
      std::unique_lock<std::mutex>  lock(m_jobsMutex);
      m_workSemaphore.Wait(lock);

      if (m_jobs.empty() && m_finishing)
      {
        break;
      }

      // Take the next job
      XR_ASSERT(Worker, !m_jobs.empty());
      job = m_jobs.front();
      m_jobs.pop_front();
    }

    job->Start();

    bool done = false;
    do
    {
      done = job->Process();
    }
    while (!done);
  }
}

} // XR
