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
    static void JobFn(void* pData)
    {
      int* pInt = static_cast<int*>(pData);
      if (pInt)
      {
        if (*pInt > 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(*pInt));
        }

        ++*pInt;
      }
    }

    TEST_METHOD(Worker_JobDone_Finalize)
    {
      auto worker = std::make_unique<Worker>();
      worker->SetIdleThreadExpiry(0, 0);

      int i = 100;
      worker->Enqueue({ JobFn, &i });
      worker->Finalize();
      worker.reset();

      Assert::IsTrue(i == 101);
    }

    TEST_METHOD(Worker_CancelPendingJobs)
    {
      auto worker = std::make_unique<Worker>();
      worker->SetIdleThreadExpiry(0, 0);

      int i = 100;
      const int kNumIterations = 3;
      for(int j = 0; j < kNumIterations; ++j)
      {
        worker->Enqueue({ JobFn, &i });
      }
      worker->CancelPendingJobs();
      auto workerGuard = MakeScopeGuard([&worker]()
      {
        worker->Finalize();
      });

      Assert::IsTrue(i < 100 + kNumIterations);
    }
  };
}