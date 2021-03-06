//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/threading/Worker.hpp"
#include "xr/memory/ScopeGuard.hpp"
#include <list>
#include <chrono>

using namespace xr;

namespace
{

static void BusyWait(int ms)
{
  const auto until = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
  while (std::chrono::system_clock::now() < until)
  {
    ++ms;
  }
}

struct Job: Worker::Job
{
  int durationMs;
  int cancelled = 0;
  int chunks = 1;

  bool Process()
  {
    std::unique_lock<decltype(Job::mutex)> lock(mutex);
    BusyWait(durationMs);

    ++executed;
    return executed % chunks == 0;
  }

  void Cancel()
  {
    std::unique_lock<decltype(Job::mutex)> lock(mutex);
    ++cancelled;
  }

  int GetExecutedCount() const
  {
    std::unique_lock<decltype(Job::mutex)> lock(mutex);
    return executed;
  }

private:
  int executed = 0;
  mutable std::mutex mutex; // we're operating on the same instance, might be executing and cancelling at the same time.
};

XM_TEST(Worker, SuspendResume)
{
  auto worker = std::make_unique<Worker>();
  auto guard = MakeScopeGuard([&] {
    worker->Finalize();
  });

  Job j;
  j.chunks = 40;
  j.durationMs = 20;

  worker->Suspend();
  worker->Enqueue(j);
  BusyWait(j.durationMs * 2);
  // don't start while suspended
  XM_ASSERT_EQ(j.GetExecutedCount(), 0);

  worker->Resume();
  BusyWait(j.durationMs * 8);

  worker->Suspend();

  const int testExec = j.GetExecutedCount();
  // suspend before finishing
  XM_ASSERT_LT(testExec, j.chunks);

  BusyWait(j.durationMs * 8);

  // don't progress while suspended
  XM_ASSERT_EQ(j.GetExecutedCount(), testExec);
  worker->Resume();

  guard.Release();
  worker->Finalize();

  // have it finished by now.
  XM_ASSERT_EQ(j.GetExecutedCount(), j.chunks);
}

XM_TEST(Worker, CancelPendingJobs)
{
  auto worker = std::make_unique<Worker>();
  auto workerGuard = MakeScopeGuard([&worker]()
  {
    worker->Finalize();
  });

  Job j;
  j.durationMs = 100;
  const int kNumIterations = 5;
  for(int i = 0; i < kNumIterations; ++i)
  {
    worker->Enqueue(j);
  }

  worker->CancelPendingJobs();

  // We don't know how many it will manage to get through, but it can't be all of them.
  XM_ASSERT_GT(j.cancelled, 0);

  const int testExec = j.GetExecutedCount();
  const auto executedPlusCancelled = testExec + j.cancelled;
  XR_TRACEIF(Worker, executedPlusCancelled != kNumIterations,
    ("%d enqueued, %d executed, %d cancelled does not add up.", kNumIterations, testExec, j.cancelled));
  XM_ASSERT_EQ(executedPlusCancelled, kNumIterations);
}

XM_TEST(Worker, EnqueueBeforeFinalize)
{
  auto worker = std::make_unique<Worker>();

  Job j;
  j.durationMs = 20;

  const int kNumIterations = 10;
  for (int i = 0; i < kNumIterations; ++i)
  {
    worker->Enqueue(j);
  }
  worker->Finalize();
  XM_ASSERT_EQ(j.GetExecutedCount(), kNumIterations);

  worker.reset();
}

XM_TEST(Worker, EnqueueAfterFinalize)
{
  auto worker = std::make_unique<Worker>();

  Job j;
  j.durationMs = 250;
  worker->Enqueue(j);
  worker->Finalize();
  XM_ASSERT_EQ(j.GetExecutedCount(), 1);

  worker->Enqueue(j);
  XM_ASSERT_EQ(j.cancelled, 1);
  XM_ASSERT_EQ(j.GetExecutedCount(), 1);
  worker->Finalize();
  worker.reset();
}

struct SpawnerJob : Worker::Job
{
  int depth;
  Worker* worker;

  std::atomic<int>* hits;

  std::list<SpawnerJob>* jobs;

  // Inherited via Job
  virtual bool Process() override
  {
    if (depth > 0)
    {
      SpawnerJob j(*this);
      --j.depth;

      jobs->push_back(j);

      worker->Enqueue(jobs->back());
    }
    ++(*hits);
    return true;
  }
};

XM_TEST(Worker, EnqueueMultithreaded)
{
  auto worker = std::make_unique<Worker>();

  std::atomic<int> hits(0);
  std::list<SpawnerJob> jobs;

  SpawnerJob j;
  j.depth = 9000;
  j.worker = worker.get();
  j.hits = &hits;
  j.jobs = &jobs;

  worker->Enqueue(j);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  worker->Finalize();
  worker.reset();

  XM_ASSERT_EQ(hits, j.depth + 1);
}

}
