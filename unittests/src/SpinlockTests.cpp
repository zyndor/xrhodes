#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/SpinLock.hpp>
#include <vector>
#include <mutex>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(SpinlockTests)
  {
    struct Data
    {
      Spinlock spinner;

      int accessCounter = 0;
      std::vector<int> data;
      
      std::chrono::time_point<std::chrono::system_clock> tStart;

      Data(size_t n)
      : data(n),
        tStart(std::chrono::system_clock::now())
      {}

      ~Data()
      {
        std::chrono::duration<double> duration = std::chrono::system_clock::now() - tStart;
        auto elapsed = duration.count();
        char arBuffer[64];
        sprintf(arBuffer, "Time taken: %fms", elapsed);
        Logger::WriteMessage(arBuffer);
      }

      void Assert(int n)
      {
        Assert::IsTrue(accessCounter == n * data.size());
        for (int i = 0; i < data.size(); ++i)
        {
          Assert::IsTrue(data[i] == n);
        }
      }

      void ThreadWhole(int id, int n)
      {
        std::unique_lock<Spinlock> lock(spinner);
        for (int i = 0; i < n; ++i)
        {
          ++data[id];
          ++accessCounter;
        }
      }

      void ThreadPerAccess(int id, int n)
      {
        for (int i = 0; i < n; ++i)
        {
          {
            std::unique_lock<Spinlock> lock(spinner);
            ++data[id];
            ++accessCounter;
          }

          using namespace std::chrono_literals;
          std::this_thread::sleep_for(10ms);
        }
      }
    };

    TEST_METHOD(Spinlock_WholeThread)
    {
      int numAccesses = 256000;
      Data data(2);

      std::vector<std::thread> threads;
      threads.reserve(data.data.size());
      for (int i = 0; i < threads.capacity(); ++i)
      {
        threads.emplace_back(&Data::ThreadWhole, &data, i, numAccesses);
      }

      for (int i = 0; i < threads.capacity(); ++i)
      {
        threads[i].join();
      }

      data.Assert(numAccesses);
    }

    TEST_METHOD(Spinlock_PerAccess)
    {
      int numAccesses = 1000;
      Data data(2);

      std::vector<std::thread> threads;
      threads.reserve(data.data.size());
      for (int i = 0; i < threads.capacity(); ++i)
      {
        threads.emplace_back(&Data::ThreadPerAccess, &data, i, numAccesses);
      }

      for (int i = 0; i < threads.capacity(); ++i)
      {
        threads[i].join();
      }

      data.Assert(numAccesses);
    }

  };

}
