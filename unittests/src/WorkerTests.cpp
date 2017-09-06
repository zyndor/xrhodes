#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Worker.hpp>
#include <XR/ScopeGuard.hpp>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(WorkerTests)
  {
  public:
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

    TEST_METHOD(Worker_SuspendResume)
    {
      auto worker = std::make_unique<Worker>();
      auto guard = MakeScopeGuard([&] {
        worker->Finalize();
      });

      Job j;
      j.chunks = 50;
      j.durationMs = 20;

      worker->Suspend();
      worker->Enqueue(j);
      BusyWait(j.durationMs * 2);
      Assert::IsTrue(j.GetExecutedCount() == 0, L"Expected to not start any while suspended.");

      worker->Resume();
      BusyWait(j.durationMs * 8);

      worker->Suspend();

      const int testExec = j.GetExecutedCount();
      Assert::IsTrue(testExec < j.chunks, L"Expected to suspend before finish.");

      BusyWait(j.durationMs * 8);

      Assert::IsTrue(j.GetExecutedCount() == testExec, L"Expected to have not progressed with the suspended job.");
      worker->Resume();

      guard.Release();
      worker->Finalize();

      Assert::IsTrue(j.GetExecutedCount() == j.chunks, L"Expected to have finished at this point.");
    }


    TEST_METHOD(Worker_CancelPendingJobs)
    {
      auto worker = std::make_unique<Worker>();

      Job j;
      j.durationMs = 100;
      const int kNumIterations = 5;
      for(int i = 0; i < kNumIterations; ++i)
      {
        worker->Enqueue(j);
      }

      worker->CancelPendingJobs();
      auto workerGuard = MakeScopeGuard([&worker]()
      {
        worker->Finalize();
      });

      // We don't know how many it will manage to get through, but it can't be all of them.
      Assert::IsTrue(j.cancelled > 0);

      wchar_t buffer[256];
      const int testExec = j.GetExecutedCount();
      swprintf(buffer, L"%d enqueued, %d executed, %d cancelled does not add up.", kNumIterations, testExec, j.cancelled);
      Assert::IsTrue(testExec + j.cancelled == kNumIterations, buffer);
    }

    TEST_METHOD(Worker_JobDone_Finalize)
    {
      auto worker = std::make_unique<Worker>();

      Job j;
      j.durationMs = 100;
      worker->Enqueue(j);
      worker->Finalize();
      worker.reset();

      Assert::IsTrue(j.GetExecutedCount() == 1);
    }
  };
}
