//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/math/Quaternion.hpp"

using namespace xr;

namespace xr
{

bool operator==(Quaternion const& q, Quaternion const& r)
{
  for (auto m : { &Quaternion::i, &Quaternion::j, &Quaternion::k, &Quaternion::w })
  {
    if (std::abs(r.*m - q.*m) >= kEpsilon)
    {
      return false;
    }
  }
  return true;
}

}

namespace
{

// NOTE: some of the relative quaternion orientation checks may fail in optimised
// builds while the values printed to stdout are the same (i.e. minuscule errors).
#define TEST_ASSERT_FEQ(a, b) XM_ASSERT_LT(std::abs(a - b), kEpsilon) // TODO: move to XM

XM_TEST(Quaternion, Normalise)
{
  Quaternion q(1.f, -5.f, 8000.f, 942.1234f);
  XM_ASSERT_NE(q.Magnitude(), 1.f);

  q.Normalise();
  XM_ASSERT_EQ(q.Magnitude(), 1.f);
}

XM_TEST(Quaternion, Identity)
{
  auto q = Quaternion::FromMatrix(Matrix::Identity());
  XM_ASSERT_EQ(q, Quaternion::Identity());
  XM_ASSERT_EQ(q.Magnitude(), 1.f);
}

XM_TEST(Quaternion, RotateAlongX)
{
  auto qAa = Quaternion::FromAxisAngle(Vector3::UnitX(), kPiHalf);

  XM_ASSERT_EQ(qAa.Magnitude(), 1.f);
  XM_ASSERT_EQ(qAa.Rotate(Vector3::UnitX()).Dot(Vector3::UnitX()), 1.f);
  TEST_ASSERT_FEQ(1.f, qAa.Rotate(Vector3::UnitY()).Dot(-Vector3::UnitZ()));
  TEST_ASSERT_FEQ(0.f, qAa.Rotate(Vector3::UnitY()).Dot(Vector3::UnitY()));

  auto qPyr = Quaternion::FromPitchYawRoll(Vector3(kPiHalf, 0.f, 0.f));
  XM_ASSERT_EQ(qAa, qPyr);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPyr));

  auto qPos = Quaternion::FromPositions(Vector3::UnitZ(), Vector3::UnitY());
  XM_ASSERT_EQ(qAa, qPos);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPos));

  Matrix m;
  m.SetRotationX(kPiHalf, true);
  auto qM = Quaternion::FromMatrix(m);
  XM_ASSERT_EQ(qAa, qM);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qM));

  auto euler = qM.ToEuler();
  TEST_ASSERT_FEQ(kPiHalf, euler.x);
}

XM_TEST(Quaternion, RotateAlongY)
{
  auto qAa = Quaternion::FromAxisAngle(Vector3::UnitY(), kPiHalf);

  XM_ASSERT_EQ(qAa.Magnitude(), 1.f);
  XM_ASSERT_EQ(qAa.Rotate(Vector3::UnitY()).Dot(Vector3::UnitY()), 1.f);
  TEST_ASSERT_FEQ(1.f, qAa.Rotate(Vector3::UnitZ()).Dot(-Vector3::UnitX()));
  TEST_ASSERT_FEQ(0.f, qAa.Rotate(Vector3::UnitZ()).Dot(Vector3::UnitZ()));

  auto qPyr = Quaternion::FromPitchYawRoll(Vector3(0.f, kPiHalf, 0.f));
  XM_ASSERT_EQ(qAa, qPyr);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPyr));

  auto qPos = Quaternion::FromPositions(Vector3::UnitX(), Vector3::UnitZ());
  XM_ASSERT_EQ(qAa, qPos);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPos));

  Matrix m;
  m.SetRotationY(kPiHalf, true);
  auto qM = Quaternion::FromMatrix(m);
  XM_ASSERT_EQ(qAa, qM);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qM));

  auto euler = qM.ToEuler();
  XM_ASSERT_LE(std::abs(euler.y - kPiHalf), 1.f / 2890.625f); // near asin() singularity; reduced precision is acceptable.
}

XM_TEST(Quaternion, RotateAlongZ)
{
  auto qAa = Quaternion::FromAxisAngle(Vector3::UnitZ(), kPiHalf);

  XM_ASSERT_EQ(qAa.Magnitude(), 1.f);
  XM_ASSERT_EQ(qAa.Rotate(Vector3::UnitZ()).Dot(Vector3::UnitZ()), 1.f);
  TEST_ASSERT_FEQ(1.f, qAa.Rotate(Vector3::UnitX()).Dot(-Vector3::UnitY()));
  TEST_ASSERT_FEQ(0.f, qAa.Rotate(Vector3::UnitX()).Dot(Vector3::UnitX()));

  auto qPyr = Quaternion::FromPitchYawRoll(Vector3(0.f, 0.f, kPiHalf));
  XM_ASSERT_EQ(qAa, qPyr);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPyr));

  auto qPos = Quaternion::FromPositions(Vector3::UnitY(), Vector3::UnitX());
  XM_ASSERT_EQ(qAa, qPos);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qPos));

  Matrix m;
  m.SetRotationZ(kPiHalf, true);
  auto qM = Quaternion::FromMatrix(m);
  XM_ASSERT_EQ(qAa, qM);
  TEST_ASSERT_FEQ(1.f, qAa.Dot(qM));

  auto euler = qM.ToEuler();
  TEST_ASSERT_FEQ(kPiHalf, euler.z);
}

}
