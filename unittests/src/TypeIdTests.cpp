//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
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

TEST(TypeId, Basics)
{
  ASSERT_TRUE(TypeId<FooBase>() == TypeId<FooBase>());
  ASSERT_TRUE(TypeId<Foo>() != TypeId<FooBase>());
  ASSERT_TRUE(TypeId<MyFoo>() == TypeId<Foo>());

  ASSERT_TRUE(TypeId<FooT<int>>() != TypeId<FooT<char>>());
  ASSERT_TRUE(TypeId<Foo>() != TypeId<FooT<int>>());
  ASSERT_TRUE(TypeId<FooBase>() != TypeId<FooT<int>>());
  ASSERT_TRUE(TypeId<Foo>() != TypeId<FooT<char>>());
  ASSERT_TRUE(TypeId<FooBase>() != TypeId<FooT<char>>());
}

}
