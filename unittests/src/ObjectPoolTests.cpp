//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/memory/ObjectPool.hpp"

namespace
{

struct TestObject: XR_OBJECTPOOL_CLIENT(TestObject)
{
  float floats[5];
  char chars[33];
};

class ObjectPool
{
public:
  static constexpr uint32_t kCapacity = 1024;

  ObjectPool()
  {
    xr::ObjectPool<TestObject>::Init(kCapacity);
  }

  ~ObjectPool()
  {
    xr::ObjectPool<TestObject>::Shutdown();
  }
};

XM_TEST_F(ObjectPool, Basics)
{
  auto obj = new TestObject();
  XM_ASSERT_NE(obj, nullptr);

  // Allocations are in contiguous memory, but at least in order.
  for (uint32_t i = 0; i < ObjectPool::kCapacity - 1; ++i)
  {
    XM_ASSERT_GT(new TestObject(), obj + i);
  }

  // Allocating beyond capacity will fail with a nullptr.
  XM_ASSERT_EQ(new TestObject(), nullptr);

  // Release a bunch of handles from the middle.
  for (uint32_t i = 0; i < ObjectPool::kCapacity / 2; ++i)
  {
    delete (obj + ObjectPool::kCapacity / 4 + i);
  }

  // Subsequent allocations will yield previously seen objects in the reverse order of their release.
  for (uint32_t i = 0; i < ObjectPool::kCapacity / 2; ++i)
  {
    XM_ASSERT_EQ(new TestObject(), obj + (3 * (ObjectPool::kCapacity / 4) - (i + 1)));
  }

  // Delete everything to prevent the leaked handles warning.
  for (uint32_t i = 0; i < ObjectPool::kCapacity; ++i)
  {
    delete obj;
  }
}

}
