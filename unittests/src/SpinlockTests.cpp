#include <gtest/gtest.h>
#include <XR/SpinLock.hpp>
#include <XR/debug.hpp>
#include <vector>
#include <mutex>
#include <chrono>

namespace XR
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
      XR_TRACE(SpinlockTests, (arBuffer));
    }

    void Assert(int n)
    {
      ASSERT_EQ(accessCounter, n * data.size());
      for (size_t i = 0; i < data.size(); ++i)
      {
        ASSERT_EQ(data[i], n);
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

  TEST(Spinlock, WholeThread)
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

  TEST(Spinlock, PerAccess)
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
}