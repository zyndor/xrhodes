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
: m_numAttempts(100),
  m_sleepIntervalMs(10)
{}

//==============================================================================
void Worker::SetIdleThreadExpiry(int numAttempts, int sleepIntervalMs)
{
  XR_ASSERT(Worker, numAttempts >= 0);
  XR_ASSERT(Worker, sleepIntervalMs > 0 || (sleepIntervalMs * numAttempts == 0));
  m_numAttempts = numAttempts;
  m_sleepIntervalMs = sleepIntervalMs;
}

//==============================================================================
void  Worker::Enqueue(Job j)
{
  if (m_thread.joinable())
  {
    std::unique_lock<std::mutex> lock(m_jobsMutex);
    m_jobs.push_back(j);
  }
  else
  {
    m_jobs.push_back(j);
    m_thread = std::thread(ThreadFunction, MakeRefHolder(*this));
  }
}

//==============================================================================
void  Worker::Finalize()
{
  if (m_thread.joinable())
  {
    m_thread.join();
  }
}

//==============================================================================
void  Worker::Loop()
{
  const int numAttempts = m_numAttempts;
  const auto sleepInterval = std::chrono::milliseconds(m_sleepIntervalMs);
  int attemptsLeft = m_numAttempts;
  while (true)
  {
    // check if we have jobs
    bool hasJob;
    {
      std::unique_lock<std::mutex>  lock(m_jobsMutex);
      hasJob = !m_jobs.empty();
    }

    if (hasJob)
    {
      // reset number of attemptsLeft
      attemptsLeft = numAttempts;

      // access job
      auto& j = m_jobs.front();
      auto jobGuard = MakeScopeGuard([this]()
      {
        // remove from queue
        std::unique_lock<std::mutex>  lock(m_jobsMutex);
        m_jobs.pop_front();
      });

      // perform job
      (*j.pJobCb)(j.pData);
    }
    else
    {
      // decrease number of attemptsLeft.
      --attemptsLeft;
      if (attemptsLeft > 0)
      {
        std::this_thread::sleep_for(sleepInterval);
      }
      else
      {
        // we've been idle for too long, thread's going to finish now.
        break;
      }
    }
  }
}

} // XR
