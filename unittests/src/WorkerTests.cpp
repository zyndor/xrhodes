#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Worker.hpp>
#include <XR/ScopeGuard.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(WorkerTests)
  {
  public:
    struct Job
    {
      int durationMs;
      int executed = 0;
      int cancelled = 0;
      std::mutex mutex; // we're operating on the same instance, might be executing and cancelling at the same time.
    };

    static void JobFn(void* pData)
    {
      Job* j = static_cast<Job*>(pData);
      std::unique_lock<decltype(Job::mutex)> lock(j->mutex);
      if (j->durationMs > 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(j->durationMs));
      }

      ++j->executed;
    }

    TEST_METHOD(Worker_JobDone_Finalize)
    {
      auto worker = std::make_unique<Worker>();

      Job j;
      j.durationMs = 100;
      worker->Enqueue({ JobFn, nullptr, &j });
      worker->Finalize();
      worker.reset();

      Assert::IsTrue(j.executed == 1);
    }

    static void CancelFn(void* pData)
    {
      Job* j = static_cast<Job*>(pData);
      std::unique_lock<decltype(Job::mutex)> lock(j->mutex);
      ++j->cancelled;
    }

    TEST_METHOD(Worker_CancelPendingJobs)
    {
      auto worker = std::make_unique<Worker>();

      Job j;
      j.durationMs = 100;
      const int kNumIterations = 5;
      for(int i = 0; i < kNumIterations; ++i)
      {
        worker->Enqueue({ JobFn, CancelFn, &j });
      }

      worker->CancelPendingJobs();
      auto workerGuard = MakeScopeGuard([&worker]()
      {
        worker->Finalize();
      });

      // We don't know how many it will manage to get through, but it can't be all of them.
      Assert::IsTrue(j.cancelled > 0);

      wchar_t buffer[256];
      swprintf(buffer, L"%d enqueued, %d executed, %d cancelled does not add up.", kNumIterations, j.executed, j.cancelled);
      Assert::IsTrue(j.executed + j.cancelled == kNumIterations, buffer);
    }
  };
}
