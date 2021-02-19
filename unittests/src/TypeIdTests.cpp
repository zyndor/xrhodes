//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/types/typeutils.hpp"

using namespace xr;

namespace
{

struct FooBase
{
  virtual ~FooBase() {}
};

struct Foo : public FooBase
{};

using MyFoo = Foo;

template <typename T>
struct FooT
{
  T* data;
};

XM_TEST(TypeId, Basics)
{
  XM_ASSERT_TRUE(TypeId<FooBase>() == TypeId<FooBase>());
  XM_ASSERT_TRUE(TypeId<Foo>() != TypeId<FooBase>());
  XM_ASSERT_TRUE(TypeId<MyFoo>() == TypeId<Foo>());

  XM_ASSERT_TRUE(TypeId<FooT<int>>() != TypeId<FooT<char>>());
  XM_ASSERT_TRUE(TypeId<Foo>() != TypeId<FooT<int>>());
  XM_ASSERT_TRUE(TypeId<FooBase>() != TypeId<FooT<int>>());
  XM_ASSERT_TRUE(TypeId<Foo>() != TypeId<FooT<char>>());
  XM_ASSERT_TRUE(TypeId<FooBase>() != TypeId<FooT<char>>());
}

}
