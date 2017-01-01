//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <SDL_thread.h>
#include <XR/Worker.hpp>

namespace XR
{

int StartRoutine( void* p)
{
  Worker::Core      *pWorker(static_cast<Worker::Core*>(p));
  const bool&       isQuitting(pWorker->isQuitting);
  Worker::JobQueue& jobs(pWorker->jobs);
  SDL_mutex*        pLock(static_cast<SDL_mutex*>(pWorker->pLockImpl));

  while (!isQuitting || !jobs.empty())
  {
    SDL_mutexP(pLock);
    Worker::Job j(jobs.front());
    jobs.pop_front();
    SDL_mutexV(pLock);

    (*j.pJobCb)(j.pData);
  }

  return 0;
}

Worker::Worker(int stackSize)
: m_core()
{
  XR_ASSERT(Worker, stackSize > 0);
  m_core.pStack = new char[stackSize];
  m_core.pLockImpl = SDL_CreateMutex();
  m_core.pThreadImpl = SDL_CreateThread(StartRoutine, "XR Worker", this);
}

Worker::~Worker()
{
  m_core.isQuitting = true;

  SDL_WaitThread(static_cast<SDL_Thread*>(m_core.pThreadImpl), 0);
  SDL_DestroyMutex(static_cast<SDL_mutex*>(m_core.pLockImpl));
  delete[] static_cast<char*>(m_core.pStack);
}

bool Worker::IsBusy() const
{
  SDL_mutex*  pLock(static_cast<SDL_mutex*>(m_core.pLockImpl));
  SDL_mutexP(pLock);
  bool  result(!m_core.jobs.empty());
  SDL_mutexV(pLock);
  return result;
}

void Worker::Do( JobCallback pJobCb, void* pData )
{
  XR_ASSERT(Worker, pJobCb != 0);

  Job j = { pJobCb, pData };

  SDL_mutex*  pLock(static_cast<SDL_mutex*>(m_core.pLockImpl));
  SDL_mutexP(pLock);
  m_core.jobs.push_back(j);
  SDL_mutexV(pLock);
}

void  Worker::Abort()
{
  SDL_mutex*  pLock(static_cast<SDL_mutex*>(m_core.pLockImpl));
  SDL_mutexP(pLock);
  m_core.jobs.clear();
  SDL_mutexV(pLock);
}

} // XR
