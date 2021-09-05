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

  XM_ASSERT_EQ(serv.GetCapacity(), 4u);
  XM_ASSERT_EQ(serv.GetNumActive(), 0u);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 0u);

  // acquiring should give you indices in ascending order
  XM_ASSERT_EQ(serv.Acquire(), 0u);
  XM_ASSERT_EQ(serv.GetNumActive(), 1u);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 1u);
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_EQ(serv.Acquire(), 1u);
  XM_ASSERT_EQ(serv.GetNumActive(), 2u);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 2u);
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_EQ(serv.Acquire(), 2u);
  XM_ASSERT_EQ(serv.GetNumActive(), 3u);
  XM_ASSERT_EQ(serv.GetNumAcquired(), 3u);
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_EQ(serv.Acquire(), 3u);
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // increasing with each allocation.
  XM_ASSERT_EQ(serv.GetNumAcquired(), 4u); // increasing with each allocation.
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);


  // until the capacity has depleted - should return invalid index.
  XM_ASSERT_EQ(serv.Acquire(), IndexServer::kInvalidIndex);

  XM_ASSERT_TRUE(serv.Release(0));
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 3u); // decreasing with releases
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_TRUE(serv.Release(3));
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 2u); // decreasing with releases
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_TRUE(serv.Release(1));
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 1u); // decreasing with releases
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  XM_ASSERT_TRUE(serv.Release(2));
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // never decreasing
  XM_ASSERT_EQ(serv.GetNumAcquired(), 0u); // decreasing with releases
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  // after releasing, the first one we will acquire is the least recently released
  XM_ASSERT_EQ(serv.Acquire(), 2u);
  XM_ASSERT_EQ(serv.Acquire(), 1u);
  XM_ASSERT_EQ(serv.Acquire(), 3u);
  XM_ASSERT_EQ(serv.Acquire(), 0u);
  XM_ASSERT_EQ(serv.GetNumActive(), 4u); // no change
  XM_ASSERT_EQ(serv.GetNumAcquired(), 4u); // and back up with allocations
  XM_ASSERT_EQ(serv.GetCapacity(), 4u);

  // duplicate releases should return false
  serv.Release(2);
  XM_ASSERT_FALSE(serv.Release(2));
}

}
