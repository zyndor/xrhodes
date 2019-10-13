//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Animator.hpp"
#include "xr/math/mathutils.hpp"

using namespace xr;

const size_t kAnimatorSize = 32;

const float kDuration = 1.f;

const auto kSetter = [](float x, void* data) {
  *static_cast<float*>(data) = x;
};

const auto kOnStop = [](void* data) {
  ++*static_cast<int*>(data);
};

FunctionPtrCallback<void, float> MakeSetter(float& f)
{
  return FunctionPtrCallback<void, float>(kSetter, &f);
}

FunctionPtrCallback<void> MakeOnStop(int& stops)
{
  return FunctionPtrCallback<void>(kOnStop, &stops);
}

TEST(Animator, Animate)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f));

  animator.Update(kDuration * .5f);
  ASSERT_EQ(f, .5f);
}

TEST(Animator, OnStop)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration);
  ASSERT_EQ(f, 1.f);
  ASSERT_EQ(stops, 1);
}

TEST(Animator, StopNoComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  auto handle = animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration * .5f);
  animator.Stop(handle, false);
  animator.Update(kDuration * .5f);

  ASSERT_EQ(f, .5f);
  ASSERT_EQ(stops, 0);
}

TEST(Animator, StopComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  auto handle = animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration * .5f);
  animator.Stop(handle, true);
  animator.Update(kDuration * .25f);

  ASSERT_EQ(f, 1.0f);
  ASSERT_EQ(stops, 1);
}

TEST(Animator, ClearNoComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  float g = 1.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);
  animator.Animate(kDuration, g, 0.f, Lerp, MakeSetter(g), &onStop);

  animator.Update(kDuration * .5f);
  animator.Clear(false);
  float h = 0.f;
  animator.Animate(kDuration, h, 10.f, Lerp, MakeSetter(h), &onStop);
  animator.Update(kDuration * .25f);

  ASSERT_EQ(f, .5f);
  ASSERT_EQ(g, .5f);
  ASSERT_EQ(h, 2.5f);
  ASSERT_EQ(stops, 0);
}

TEST(Animator, ClearComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  float g = 1.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);
  animator.Animate(kDuration, g, 0.f, Lerp, MakeSetter(g), &onStop);

  animator.Update(kDuration * .5f);
  animator.Clear(true);
  float h = 0.f;
  animator.Animate(kDuration, h, 10.f, Lerp, MakeSetter(h), &onStop);
  animator.Update(kDuration * .25f);

  ASSERT_EQ(f, 1.f);
  ASSERT_EQ(g, 0.f);
  ASSERT_EQ(h, 2.5f);
  ASSERT_EQ(stops, 2);
}
