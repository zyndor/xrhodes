//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/memory/IndexServer.hpp"
#include <memory>

using namespace xr;

namespace
{

XM_TEST(IndexServer, Basics)
{
  IndexServer serv(4);

  // acquiring should give you indices in ascending order
  XM_ASSERT_EQ(serv.GetNumActive(), 0);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 0);
  XM_ASSERT_EQ(serv.Acquire(), 0);
  XM_ASSERT_EQ(serv.GetNumActive(), 1);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 1);
  XM_ASSERT_EQ(serv.Acquire(), 1);
  XM_ASSERT_EQ(serv.GetNumActive(), 2);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 2);
  XM_ASSERT_EQ(serv.Acquire(), 2);
  XM_ASSERT_EQ(serv.GetNumActive(), 3);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 3);
  XM_ASSERT_EQ(serv.Acquire(), 3);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // increasing with each allocation.
  XM_ASSERT_EQ(serv.GetNumAcquired(), 4); // increasing with each allocation.

  // until the capacity has depleted - should fail with an exception
  XM_ASSERT_THROW(serv.Acquire(), std::bad_alloc);

  serv.Release(0);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 3); // decreasing with releases
  serv.Release(3);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 2); // decreasing with releases
  serv.Release(1);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 1); // decreasing with releases
  serv.Release(2);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 0); // decreasing with releases
  // after releasing, the first one we will acquire is the least recently released
  XM_ASSERT_EQ(serv.Acquire(), 2);
  XM_ASSERT_EQ(serv.Acquire(), 1);
  XM_ASSERT_EQ(serv.Acquire(), 3);
  XM_ASSERT_EQ(serv.Acquire(), 0);
  XM_ASSERT_EQ(serv.GetNumActive(), 4); // no change
  XM_ASSERT_EQ(serv.GetNumAcquired(), 4); // and back up with allocations

  // duplicate releases should fail with an exceptions
  serv.Release(2);
  XM_ASSERT_THROW(serv.Release(2), std::logic_error);
}

}
