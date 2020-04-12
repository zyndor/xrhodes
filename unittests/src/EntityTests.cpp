//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Entity.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

class EntityTests: public ::testing::Test
{
public:
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}
};

TEST_F(EntityTests, AddRemoveDetach)
{
  Entity eRoot(Name("root"), nullptr);
  ASSERT_EQ(eRoot.GetChildren().size(), 0);

  auto eA = new Entity(Name("A"), &eRoot);
  ASSERT_EQ(eA->GetParent(), &eRoot);
  ASSERT_EQ(eRoot.GetChildren().size(), 1);
  ASSERT_EQ(eA->GetChildren().size(), 0);

  auto eB = new Entity(Name("B"), eA);
  ASSERT_EQ(eB->GetParent(), eA);
  ASSERT_EQ(eB->GetChildren().size(), 0);

  auto eC = new Entity(Name("C"), eA);
  ASSERT_EQ(eC->GetParent(), eA);
  ASSERT_EQ(eC->GetChildren().size(), 0);
  ASSERT_EQ(eA->GetChildren().size(), 2);

  eRoot.AddChild(*eC);
  ASSERT_EQ(eC->GetParent(), &eRoot);
  ASSERT_EQ(eRoot.GetChildren().size(), 2);
  ASSERT_EQ(eA->GetChildren().size(), 1);

  eA->RemoveChild(*eC);
  ASSERT_EQ(eA->GetChildren().size(), 1);

  eC->DetachFromParent();
  ASSERT_EQ(eC->GetParent(), nullptr);
  ASSERT_EQ(eRoot.GetChildren().size(), 1);
  delete eC;
}

TEST_F(EntityTests, FindChild)
{
  Entity eRoot(Name("root"), nullptr);
  auto eA = new Entity(Name("A"), &eRoot);
  auto eB = new Entity(Name("B"), eA);
  auto eC = new Entity(Name("C"), eA);
  auto eC2 = new Entity(Name("C"), eB);

  ASSERT_EQ(eRoot.FindChild(Name("C")), nullptr);
  ASSERT_EQ(eRoot.FindChild("A.C"), eC);
  ASSERT_EQ(eRoot.FindChild("A.B.C"), eC2);
  ASSERT_EQ(eRoot.FindChild(Name("A.C")), nullptr);
  ASSERT_EQ(eRoot.FindChild(Name("A.B.C")), nullptr);

  eC->AddChild(*eC2);
  ASSERT_EQ(eRoot.FindChild("A.B.C"), nullptr);
  ASSERT_EQ(eRoot.FindChild("A.C.C"), eC2);

  eA->AddChild(*eC2);

  eC->DetachFromParent();
  ASSERT_EQ(eRoot.FindChild("A.B.C"), nullptr);
  delete eC;
}

bool IsEqual(Matrix const& m0, Matrix const& m1)
{
  float diff = (m1.t - m0.t).Dot();
  for (int i = 0; i < XR_ARRAY_SIZE(m0.linear); ++i)
  {
    diff += m1.linear[i] - m0.linear[i];
  }
  return abs(diff) < 1e-4;
}

TEST_F(EntityTests, Transforms)
{
  Entity eRoot(Name("root"), nullptr);
  eRoot.SetTranslation(Vector3::UnitX());
  eRoot.SetRotation(Quaternion::FromAxisAngle(Vector3::UnitZ(), M_PI * .5f));
  ASSERT_TRUE(IsEqual(Matrix(eRoot.GetRotation(), eRoot.GetTranslation()),
    eRoot.GetLocalTransform()));

  Entity eChild(Name("child"), &eRoot);
  eChild.SetTranslation(Vector3::UnitY());
  ASSERT_TRUE(IsEqual(Matrix(eChild.GetTranslation()), eChild.GetLocalTransform()));

  ASSERT_TRUE(IsEqual(Matrix::Identity(),
    eChild.GetWorldTransform(false)));

  Matrix expectTransform(eRoot.GetRotation(), Vector3::UnitX() * 2.0f);
  ASSERT_TRUE(IsEqual(expectTransform, eChild.GetWorldTransform(true)));
  ASSERT_TRUE(IsEqual(expectTransform, eChild.GetWorldTransform(false)));
}

class XR_COMPONENT_DECL(TestComponent)
{
public:
  int mFoo;

  TestComponent* Clone() const
  {
    auto clone = new TestComponent();
    clone->mFoo = mFoo;
    return clone;
  }
};

TEST_F(EntityTests, AddFindComponment)
{
  Entity e(nullptr);
  auto c = e.AddComponent<TestComponent>();
  ASSERT_EQ(c->GetOwner(), &e); // Owner correctly set.
  ASSERT_EQ(c, e.FindComponent<TestComponent>());

  const auto eConst = &e;
  ASSERT_EQ(c, eConst->FindComponent<TestComponent>());

  ASSERT_EQ(e.AddComponent<TestComponent>(), nullptr);  // Already have this type - do nothing.
}

template <typename T, size_t N>
void AssertEqualData(T(&d0)[N], T(&d1)[N])
{
  for(size_t i = 0; i < N; ++i)
  {
    ASSERT_EQ(d0[i], d1[i]);
  }
}

TEST_F(EntityTests, Clone)
{
  Entity e(Name("test"), nullptr);
  e.SetScale(Vector3(1., 2., 3.));
  e.SetRotation(Quaternion::FromPitchYawRoll(Vector3(1., -2., 0.)));
  e.SetTranslation(Vector3(-10., 25., 0.));

  auto c = e.AddComponent<TestComponent>();
  c->mFoo = 17;

  auto e2 = e.Clone(&e);
  ASSERT_NE(e2, &e);
  ASSERT_EQ(e2->GetName(), e.GetName());
  AssertEqualData(e2->GetTranslation().data, e.GetTranslation().data);
  AssertEqualData(e2->GetRotation().data, e.GetRotation().data);
  AssertEqualData(e2->GetScale().data, e.GetScale().data);

  ASSERT_EQ(e.GetChildren().size(), 1);
  ASSERT_EQ(e.GetFirstChild(), e2);

  ASSERT_TRUE(e2->GetChildren().empty());
  ASSERT_EQ(e2->GetParent(), &e);

  auto c2 = e2->FindComponent<TestComponent>();
  ASSERT_NE(c, c2);
  ASSERT_EQ(c->mFoo, c2->mFoo);
}

}
