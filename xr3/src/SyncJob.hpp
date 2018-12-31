#ifndef XR_SYNCJOB_HPP
#define XR_SYNCJOB_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/threading/Worker.hpp"
#include "xr/threading/Semaphore.hpp"
#include <functional>

namespace xr
{

//==============================================================================
///@brief This Job calls a function, which is provided upon construction, and
/// blocks the thread that it was posted from until its execution completes.
class SyncJob: public Worker::Job
{
public:
  // structors
  SyncJob(std::function<void()> const& jobFn)
  : mJobFn(jobFn)
  {}

  ~SyncJob()
  {
    mSemaphore.Wait();
  }

  // general
  bool Process() override
  {
    mJobFn();
    mSemaphore.Post();
    return true;
  }

private:
  // data
  Semaphore mSemaphore;
  std::function<void()> mJobFn;
};

}

#endif
