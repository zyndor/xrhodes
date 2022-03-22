//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/threading/Spinlock.hpp"
#include "xr/debug.hpp"
#include <vector>
#include <mutex>
#include <chrono>
#include <thread>

using namespace xr;

namespace
{
struct Data
{
  Spinlock spinner;

  size_t accessCounter = 0;
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
    XR_TRACE(Spinlock, (arBuffer));
  }

  void Assert(int n)
  {
    XM_ASSERT_EQ(accessCounter, n * data.size());
    for (size_t i = 0; i < data.size(); ++i)
    {
      XM_ASSERT_EQ(data[i], n);
    }
  }

  void ThreadWhole(size_t id, size_t n)
  {
    std::unique_lock<Spinlock> lock(spinner);
    for (size_t i = 0; i < n; ++i)
    {
      ++data[id];
      ++accessCounter;
    }
  }

  void ThreadPerAccess(size_t id, size_t n)
  {
    for (size_t i = 0; i < n; ++i)
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

XM_TEST(Spinlock, WholeThread)
{
  int numAccesses = 256000;
  Data data(2);

  std::vector<std::thread> threads;
  threads.reserve(data.data.size());
  for (size_t i = 0; i < threads.capacity(); ++i)
  {
    threads.emplace_back(&Data::ThreadWhole, &data, i, numAccesses);
  }

  for (size_t i = 0; i < threads.capacity(); ++i)
  {
    threads[i].join();
  }

  data.Assert(numAccesses);
}

XM_TEST(Spinlock, PerAccess)
{
  int numAccesses = 1000;
  Data data(2);

  std::vector<std::thread> threads;
  threads.reserve(data.data.size());
  for (size_t i = 0; i < threads.capacity(); ++i)
  {
    threads.emplace_back(&Data::ThreadPerAccess, &data, i, numAccesses);
  }

  for (size_t i = 0; i < threads.capacity(); ++i)
  {
    threads[i].join();
  }

  data.Assert(numAccesses);
}

}
