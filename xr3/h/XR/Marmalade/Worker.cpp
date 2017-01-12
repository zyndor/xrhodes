//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Worker.hpp"
#include <XR/debug.hpp>
#include <s3eThread.h>

namespace XR
{

void* StartRoutine( void* p)
{
  Worker::Core      *pWorker(static_cast<Worker::Core*>(p));
  const bool&       isQuitting(pWorker->isQuitting);
  Worker::JobQueue& jobs(pWorker->jobs);
  s3eThreadLock*    pLock(static_cast<s3eThreadLock*>(pWorker->pLockImpl));

  while(!isQuitting || !jobs.empty())
  {
    s3eThreadLockAcquire(pLock);
    Worker::Job j(jobs.front());
    jobs.pop_front();
    s3eThreadLockRelease(pLock);

    (*j.pJobCb)(j.pData);
  }

  return 0;
}

Worker::Worker(int stackSize)
: m_core()
{
  XR_ASSERT(Worker, s3eThreadAvailable());
  XR_ASSERT(Worker, stackSize > 0);
  m_core.pStack = new char[stackSize];
  m_core.pLockImpl = s3eThreadLockCreate();
  m_core.pThreadImpl = s3eThreadCreate(StartRoutine, this, 0, stackSize,
    m_core.pStack);
}

Worker::~Worker()
{
  m_core.isQuitting = true;

  s3eThread*  pThread(static_cast<s3eThread*>(m_core.pThreadImpl));
  s3eResult result = s3eThreadJoin(pThread, 0);

  s3eThreadLock*  pLock(static_cast<s3eThreadLock*>(m_core.pLockImpl));
  s3eThreadLockDestroy(pLock);
  delete[] static_cast<char*>(m_core.pStack);
}

void Worker::Do( JobCallback pJobCb, void* pData )
{
  XR_ASSERT(Worker, pJobCb != 0);

  Job j = { pJobCb, pData };

  s3eThreadLock*  pLock(static_cast<s3eThreadLock*>(m_core.pLockImpl));
  s3eThreadLockAcquire(pLock);

  m_core.jobs.push_back(j);
  s3eThreadLockRelease(pLock);
}

bool Worker::IsBusy() const
{
  s3eThreadLock*  pLock(static_cast<s3eThreadLock*>(m_core.pLockImpl));
  s3eThreadLockAcquire(pLock);
  bool  result(!m_core.jobs.empty());
  s3eThreadLockRelease(pLock);
  return result;
}

} // XR
