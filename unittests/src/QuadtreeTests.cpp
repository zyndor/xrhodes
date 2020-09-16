//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Quadtree.hpp"

using namespace xr;

namespace
{

struct QuadtreeTester: RectObject
{
  QuadtreeTester(float hw, float hh, Vector2 const& pos)
  : RectObject(hw, hh, pos)
  {}

  uint32_t mCounter = 0;
};

void UnaryTester(void* system)
{
  ++static_cast<QuadtreeTester*>(system)->mCounter;
}

void BinaryTester(void* system, void* user)
{
  ++static_cast<QuadtreeTester*>(system)->mCounter;
  ++static_cast<QuadtreeTester*>(user)->mCounter;
}

TEST(Quadtree, Unary)
{
  Quadtree<>  qt(Vector2::Zero(), 1024.f, 1024.f, 512.f);

  QuadtreeTester ro1(511.9f, 511.9f, Vector2::One() * 512.f);
  QuadtreeTester ro2(511.9f, 511.9f, Vector2::One() * -512.f);

  AABB box;
  ro1.Export(box);
  qt.Add(box, &ro1);

  // Process all -- one hit
  qt.ProcessAll(UnaryTester);
  ASSERT_EQ(ro1.mCounter, 1u);

  // Process with bounds of ro1 -- one hit
  ro1.mCounter = 0;
  qt.Process(box, UnaryTester);
  ASSERT_EQ(ro1.mCounter, 1u);

  // Process with bounds of ro2 -- no hit
  ro1.mCounter = 0;
  ro2.Export(box);
  qt.Process(box, UnaryTester);
  ASSERT_EQ(ro1.mCounter, 0);

  // Process with point at origin (since it's sitting between leaves, all must be checked) -- one hit
  qt.Process(Vector2::Zero(), UnaryTester);
  ASSERT_EQ(ro1.mCounter, 1u);

  // Process with point at ro1 position -- one hit
  ro1.mCounter = 0;
  qt.Process(ro1.GetPosition(), UnaryTester);
  ASSERT_EQ(ro1.mCounter, 1u);

  qt.Remove(&ro1);

  // Process with bounds of ro1 -- no hit
  ro1.Export(box);
  ro1.mCounter = 0;
  qt.Process(box, UnaryTester);
  ASSERT_EQ(ro1.mCounter, 0u);
}

TEST(Quadtree, Binary)
{
  Quadtree<>  qt(Vector2::Zero(), 1024.f, 1024.f, 512.f);

  QuadtreeTester ro1(511.9f, 511.9f, Vector2::One() * 512.f);
  QuadtreeTester ro2(511.9f, 511.9f, Vector2::One() * -512.f);

  AABB box;
  ro1.Export(box);
  qt.Add(box, &ro1);

  // Process all -- one hit
  qt.ProcessAll(BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 1u);
  ASSERT_EQ(ro2.mCounter, 1u);

  // Process with bounds of ro1 -- one hit
  ro1.mCounter = 0;
  ro2.mCounter = 0;
  qt.Process(box, BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 1u);
  ASSERT_EQ(ro2.mCounter, 1u);

  // Process with bounds of ro2 -- no hit
  ro1.mCounter = 0;
  ro2.mCounter = 0;
  ro2.Export(box);
  qt.Process(box, BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 0);
  ASSERT_EQ(ro2.mCounter, 0);

  // Process with point at origin (since it's sitting between leaves, all must be checked)  -- one hit
  qt.Process(Vector2::Zero(), BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 1u);
  ASSERT_EQ(ro2.mCounter, 1u);

  // Process with point at ro1 position -- one hit
  ro1.mCounter = 0;
  ro2.mCounter = 0;
  qt.Process(ro1.GetPosition(), BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 1u);
  ASSERT_EQ(ro2.mCounter, 1u);

  qt.Remove(&ro1);

  // Process with bounds of ro1 -- no hit
  ro1.Export(box);
  ro1.mCounter = 0;
  ro2.mCounter = 0;
  qt.Process(box, BinaryTester, &ro2);
  ASSERT_EQ(ro1.mCounter, 0u);
  ASSERT_EQ(ro2.mCounter, 0u);
}

}
