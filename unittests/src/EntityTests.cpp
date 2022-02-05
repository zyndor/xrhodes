//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/Entity.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

XM_TEST(Entity, AddRemoveDetach)
{
  xr::Entity eRoot(Name("root"), nullptr);
  XM_ASSERT_EQ(eRoot.GetChildren().size(), 0u);

  auto eA = new xr::Entity(Name("A"), &eRoot);
  XM_ASSERT_EQ(eA->GetParent(), &eRoot);
  XM_ASSERT_EQ(eRoot.GetChildren().size(), 1u);
  XM_ASSERT_EQ(eA->GetChildren().size(), 0u);

  auto eB = new xr::Entity(Name("B"), eA);
  XM_ASSERT_EQ(eB->GetParent(), eA);
  XM_ASSERT_EQ(eB->GetChildren().size(), 0u);

  auto eC = new xr::Entity(Name("C"), eA);
  XM_ASSERT_EQ(eC->GetParent(), eA);
  XM_ASSERT_EQ(eC->GetChildren().size(), 0u);
  XM_ASSERT_EQ(eA->GetChildren().size(), 2u);

  eRoot.AddChild(*eC);
  XM_ASSERT_EQ(eC->GetParent(), &eRoot);
  XM_ASSERT_EQ(eRoot.GetChildren().size(), 2u);
  XM_ASSERT_EQ(eA->GetChildren().size(), 1u);

  eA->RemoveChild(*eC);
  XM_ASSERT_EQ(eA->GetChildren().size(), 1u);

  eC->DetachFromParent();
  XM_ASSERT_EQ(eC->GetParent(), nullptr);
  XM_ASSERT_EQ(eRoot.GetChildren().size(), 1u);
  delete eC;
}

XM_TEST(Entity, FindChild)
{
  xr::Entity eRoot(Name("root"), nullptr);
  auto eA = new xr::Entity(Name("A"), &eRoot);
  auto eB = new xr::Entity(Name("B"), eA);
  auto eC = new xr::Entity(Name("C"), eA);
  auto eC2 = new xr::Entity(Name("C"), eB);

  XM_ASSERT_EQ(eRoot.FindChild(Name("C")), nullptr);
  XM_ASSERT_EQ(eRoot.FindChild("A.C"), eC);
  XM_ASSERT_EQ(eRoot.FindChild("A.B.C"), eC2);
  XM_ASSERT_EQ(eRoot.FindChild(Name("A.C")), nullptr);
  XM_ASSERT_EQ(eRoot.FindChild(Name("A.B.C")), nullptr);

  eC->AddChild(*eC2);
  XM_ASSERT_EQ(eRoot.FindChild("A.B.C"), nullptr);
  XM_ASSERT_EQ(eRoot.FindChild("A.C.C"), eC2);

  eA->AddChild(*eC2);

  eC->DetachFromParent();
  XM_ASSERT_EQ(eRoot.FindChild("A.B.C"), nullptr);
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

XM_TEST(Entity, Transforms)
{
  xr::Entity eRoot(Name("root"), nullptr);
  eRoot.SetTranslation(Vector3::UnitX());
  eRoot.SetRotation(Quaternion::FromAxisAngle(Vector3::UnitZ(), M_PI * .5f));
  XM_ASSERT_TRUE(IsEqual(Matrix(eRoot.GetRotation(), eRoot.GetTranslation()),
    eRoot.GetLocalTransform()));

  xr::Entity eChild(Name("child"), &eRoot);
  eChild.SetTranslation(Vector3::UnitY());
  XM_ASSERT_TRUE(IsEqual(Matrix(eChild.GetTranslation()), eChild.GetLocalTransform()));

  XM_ASSERT_TRUE(IsEqual(Matrix::Identity(),
    eChild.GetWorldTransform(false)));

  Matrix expectTransform(eRoot.GetRotation(), Vector3::UnitX() * 2.0f);
  XM_ASSERT_TRUE(IsEqual(expectTransform, eChild.GetWorldTransform(true)));
  XM_ASSERT_TRUE(IsEqual(expectTransform, eChild.GetWorldTransform(false)));
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

XM_TEST(Entity, AddFindComponment)
{
  xr::Entity e(nullptr);
  auto c = e.AddComponent<TestComponent>();
  XM_ASSERT_EQ(c->GetOwner(), &e); // Owner correctly set.
  XM_ASSERT_EQ(c, e.FindComponent<TestComponent>());

  const auto eConst = &e;
  XM_ASSERT_EQ(c, eConst->FindComponent<TestComponent>());

  XM_ASSERT_EQ(e.AddComponent<TestComponent>(), nullptr);  // Already have this type - do nothing.
}

template <typename T>
void AssertEqualData(T const& d0, T const& d1)
{
  XM_ASSERT_TRUE(std::equal(d0.begin(), d0.end(), d1.begin(), d1.end()));
}

XM_TEST(Entity, Clone)
{
  xr::Entity e(Name("test"), nullptr);
  e.SetScale(Vector3(1., 2., 3.));
  e.SetRotation(Quaternion::FromPitchYawRoll(Vector3(1., -2., 0.)));
  e.SetTranslation(Vector3(-10., 25., 0.));

  auto c = e.AddComponent<TestComponent>();
  c->mFoo = 17;

  auto e2 = e.Clone(&e);
  XM_ASSERT_NE(e2, &e);
  XM_ASSERT_EQ(e2->GetName(), e.GetName());
  AssertEqualData(e2->GetTranslation(), e.GetTranslation());
  AssertEqualData(e2->GetRotation(), e.GetRotation());
  AssertEqualData(e2->GetScale(), e.GetScale());

  XM_ASSERT_EQ(e.GetChildren().size(), 1u);
  XM_ASSERT_EQ(e.GetFirstChild(), e2);

  XM_ASSERT_TRUE(e2->GetChildren().empty());
  XM_ASSERT_EQ(e2->GetParent(), &e);

  auto c2 = e2->FindComponent<TestComponent>();
  XM_ASSERT_NE(c, c2);
  XM_ASSERT_EQ(c->mFoo, c2->mFoo);
}

}
