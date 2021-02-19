//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/Animator.hpp"
#include "xr/math/mathutils.hpp"

using namespace xr;

namespace
{

const size_t kAnimatorSize = 32;

const float kDuration = 1.f;

const auto kSetter = [](float x, void* data) {
  *static_cast<float*>(data) = x;
};

const auto kSetterByRef = [](float const& x, void* data) {
  *static_cast<float*>(data) = x;
};

const auto kOnStop = [](void* data) {
  ++*static_cast<int*>(data);
};

FunctionPtrCallback<void, float> MakeSetter(float& f)
{
  return FunctionPtrCallback<void, float>(kSetter, &f);
}

FunctionPtrCallback<void, float const&> MakeSetterByRef(float& f)
{
  return FunctionPtrCallback<void, float const&>(kSetterByRef, &f);
}

FunctionPtrCallback<void> MakeOnStop(int& stops)
{
  return FunctionPtrCallback<void>(kOnStop, &stops);
}

XM_TEST(Animator, Animate)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f));

  animator.Update(kDuration * .5f);
  XM_ASSERT_EQ(f, .5f);
}

XM_TEST(Animator, OnStop)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration);
  XM_ASSERT_EQ(f, 1.f);
  XM_ASSERT_EQ(stops, 1);
}

XM_TEST(Animator, StopNoComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  auto handle = animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration * .5f);
  animator.Stop(handle, false);
  animator.Update(kDuration * .5f);

  XM_ASSERT_EQ(f, .5f);
  XM_ASSERT_EQ(stops, 0);
}

XM_TEST(Animator, StopComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  auto handle = animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);

  animator.Update(kDuration * .5f);
  animator.Stop(handle, true);
  animator.Update(kDuration * .25f);

  XM_ASSERT_EQ(f, 1.0f);
  XM_ASSERT_EQ(stops, 1);
}

XM_TEST(Animator, ClearNoComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  float g = 1.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);
  animator.Animate(kDuration, g, 0.f, Lerp, MakeSetterByRef(g), &onStop);

  animator.Update(kDuration * .5f);
  animator.Clear(false);
  float h = 0.f;
  animator.Animate(kDuration, h, 10.f, Lerp, MakeSetter(h), &onStop);
  animator.Update(kDuration * .25f);

  XM_ASSERT_EQ(f, .5f);
  XM_ASSERT_EQ(g, .5f);
  XM_ASSERT_EQ(h, 2.5f);
  XM_ASSERT_EQ(stops, 0);
}

XM_TEST(Animator, ClearComplete)
{
  Animator animator(kAnimatorSize);

  float f = 0.f;
  float g = 1.f;
  int stops = 0;
  auto onStop = MakeOnStop(stops);
  animator.Animate(kDuration, f, 1.f, Lerp, MakeSetter(f), &onStop);
  animator.Animate(kDuration, g, 0.f, Lerp, MakeSetterByRef(g), &onStop);

  animator.Update(kDuration * .5f);
  animator.Clear(true);
  float h = 0.f;
  animator.Animate(kDuration, h, 10.f, Lerp, MakeSetter(h), &onStop);
  animator.Update(kDuration * .25f);

  XM_ASSERT_EQ(f, 1.f);
  XM_ASSERT_EQ(g, 0.f);
  XM_ASSERT_EQ(h, 2.5f);
  XM_ASSERT_EQ(stops, 2);
}

}
