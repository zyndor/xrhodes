//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Deflator.hpp"
#include "xr/streamutils.hpp"

namespace xr
{
  namespace
  {
    struct A : public xr::Inflatable
    {
      int a;

      // Inherited via Inflatable
      virtual void Serialize(Deflator const & deflator, std::ostream & stream) override
      {}

      virtual void Restore(std::istream & stream, Inflator & inflator) override
      {}
    };
  }

  TEST(Deflator, Basics)
  {
    A a[4];
    Deflator  deflator;

    ASSERT_EQ(deflator.RegisterObject(a[0]), 0); // first ID from deflator is 0.
    ASSERT_EQ(deflator.RegisterObject(a[0]), 0); // same object always gets the same ID.
    ASSERT_EQ(deflator.RegisterObject(a[1]), 1); // next object gets the next ID.
    ASSERT_EQ(deflator.RegisterObject(a[0]), 0); // same object always gets the same ID.

    deflator.SetNext(10);
    ASSERT_EQ(deflator.RegisterObject(a[2]), 10); // next object gets next ID.
    ASSERT_EQ(deflator.RegisterObject(a[0]), 0); // same object always gets the same ID.

    // ID is still valid and same as when registered
    ASSERT_EQ(deflator.GetId(&a[0]), 0);
    ASSERT_EQ(deflator.GetId(&a[1]), 1);
    ASSERT_EQ(deflator.GetId(&a[2]), 10);
    ASSERT_EQ(deflator.GetId(&a[3]), IdGenerator::kInvalidId); // unregistered object - gets an invalid ID.
    ASSERT_EQ(deflator.GetId(nullptr), IdGenerator::kInvalidId); // nullptrs always get an an invalid ID.
  }

  TEST(Deflator, RegisterInvalid)
  {
    A a;
    Deflator deflator;

    // Registering under reserved invalid ID shall fail.
    deflator.SetNext(IdGenerator::kInvalidId);
    ASSERT_THROW(deflator.RegisterObject(a), std::logic_error);

    // Failed registration does not result in the object being registered.
    ASSERT_EQ(deflator.GetId(&a), IdGenerator::kInvalidId);
  }

  TEST(Deflator, IdRangeClash)
  {
    Deflator deflator;
    A a[2];

    // Registration that causes ID range clash shall fail.
    deflator.RegisterObject(a[0]);
    deflator.SetNext(deflator.GetId(&a[0]));

    // Registration that causes ID clash.
    ASSERT_THROW(deflator.RegisterObject(a[1]), std::logic_error);

    // Failed registration does not result in the object being registered.
    ASSERT_EQ(deflator.GetId(&a[1]), IdGenerator::kInvalidId);
  }
}
