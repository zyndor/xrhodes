//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/memory/IndexServer.hpp"
#include <memory>

using namespace xr;

namespace
{

TEST(IndexServer, Basics)
{
  IndexServer serv(4);

  // acquiring should give you indices in ascending order
  ASSERT_EQ(serv.GetNumActive(), 0);
  ASSERT_EQ(serv.GetNumAcquired(), 0);
  ASSERT_EQ(serv.Acquire(), 0);
  ASSERT_EQ(serv.GetNumActive(), 1);
  ASSERT_EQ(serv.GetNumAcquired(), 1);
  ASSERT_EQ(serv.Acquire(), 1);
  ASSERT_EQ(serv.GetNumActive(), 2);
  ASSERT_EQ(serv.GetNumAcquired(), 2);
  ASSERT_EQ(serv.Acquire(), 2);
  ASSERT_EQ(serv.GetNumActive(), 3);
  ASSERT_EQ(serv.GetNumAcquired(), 3);
  ASSERT_EQ(serv.Acquire(), 3);
  ASSERT_EQ(serv.GetNumActive(), 4); // increasing with each allocation.
  ASSERT_EQ(serv.GetNumAcquired(), 4); // increasing with each allocation.

  // until the capacity has depleted - should fail with an exception
  ASSERT_THROW(serv.Acquire(), std::bad_alloc);

  serv.Release(0);
  ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  ASSERT_EQ(serv.GetNumAcquired(), 3); // decreasing with releases
  serv.Release(3);
  ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  ASSERT_EQ(serv.GetNumAcquired(), 2); // decreasing with releases
  serv.Release(1);
  ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  ASSERT_EQ(serv.GetNumAcquired(), 1); // decreasing with releases
  serv.Release(2);
  ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  ASSERT_EQ(serv.GetNumAcquired(), 0); // decreasing with releases
  // after releasing, the first one we will acquire is the least recently released
  ASSERT_EQ(serv.Acquire(), 2);
  ASSERT_EQ(serv.Acquire(), 1);
  ASSERT_EQ(serv.Acquire(), 3);
  ASSERT_EQ(serv.Acquire(), 0);
  ASSERT_EQ(serv.GetNumActive(), 4); // no change
  ASSERT_EQ(serv.GetNumAcquired(), 4); // and back up with allocations

  // duplicate releases should fail with an exceptions
  serv.Release(2);
  ASSERT_THROW(serv.Release(2), std::logic_error);
}

}
