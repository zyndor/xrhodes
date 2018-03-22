#ifndef XR_QUATERNION_HPP
#define XR_QUATERNION_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Matrix.hpp"
#include "XR/debug.hpp"
#include <cstring>
#include <cstdint>
#include <cmath>

namespace XR
{
//==============================================================================
enum  QuaternionInds
{
  QI,
  QJ,
  QK,
  QW,
  kNumQuaternionInds
};

//==============================================================================
class Quaternion
{
public:
  // static
  static Quaternion Unit()
  {
    return Quaternion();
  }

  ///@brief Creates a quaternion from a vector of pitch, yaw, roll rotations.
  ///@note The angles are in radians.
  static void FromPitchYawRoll(Vector3 pitchYawRoll, Quaternion& q)
  {
    pitchYawRoll *= .5f;

    const float c1(cosf(pitchYawRoll.y));
    const float c2(cosf(pitchYawRoll.x));
    const float c3(cosf(pitchYawRoll.z));
    const float s1(sinf(pitchYawRoll.y));
    const float s2(sinf(pitchYawRoll.x));
    const float s3(sinf(pitchYawRoll.z));

    const float c2c3(c2 * c3);
    const float s2s3(s2 * s3);
    const float s2c3(s2 * c3);
    const float c2s3(c2 * s3);

    q.i = s1 * c2c3 - c1 * s2s3;
    q.j = c1 * s2c3 - s1 * c2s3;
    q.k = c1 * c2s3 - s1 * s2c3;
    q.w = c1 * c2c3 - s1 * s2s3;
  }

  static Quaternion FromPitchYawRoll(Vector3 const& pitchYawRoll)
  {
    Quaternion  q;
    FromPitchYawRoll(pitchYawRoll, q);
    return q;
  }

  ///@brief Creates a quaternion that rotates by @a angle radians around the
  /// given @axis.
  ///@note The angle needs to be in radians.
  static void FromAxisAngle(Vector3 const& axis, float angle, Quaternion& q)
  {
    angle *= .5f;
    q.w = cosf(angle);

    const float s = sinf(angle);
    q.i = axis.x * s;
    q.j = axis.y * s;
    q.k = axis.z * s;
    q.Normalise();
  }

  static Quaternion FromAxisAngle(Vector3 const& axis, float angle)
  {
    Quaternion  q;
    FromAxisAngle(axis, angle, q);
    return q;
  }

  ///@brief Offers more efficient creation of a quaternion that rotates from
  /// @a from to @a to. The vectors must be normalised.
  static void FromPositionsNormalised(Vector3 const& from, Vector3 const& to, Quaternion& q)
  {
    float d = from.Dot(to);
    if (d * d < 1.0f)
    {
      Vector3 c = from.Cross(to);
      Quaternion q(c.x, c.y, c.z, d + 1.0f);
      q.Normalise();
    }
  }

  static Quaternion FromPositionsNormalised(Vector3 const& from, Vector3 const& to)
  {
    Quaternion  q;
    FromPositionsNormalised(from, to, q);
    return q;
  }

  ///@brief Creates a quaternion that rotates from @a from to @a to. These
  /// vectors will be normalised.
  static void FromPositions(Vector3 from, Vector3 to, Quaternion& q)
  {
    float dFrom = from.Dot();
    if (dFrom > .0f)
    {
      from *= 1.0f / sqrtf(dFrom);
    }

    float dTo = to.Dot();
    if (dTo > .0f)
    {
      to *= 1.0f / sqrtf(dTo);
    }

    FromPositionsNormalised(from, to, q);
  }

  static Quaternion FromPositions(Vector3 const& from, Vector3 const& to)
  {
    Quaternion  q;
    FromPositions(from, to, q);
    return q;
  }

  // data
  union
  {
    float arData[kNumQuaternionInds];
    struct
    {
      float i, j, k, w;
    };
  };

  // structors
  Quaternion()
  : i(.0f), j(.0f), k(.0f), w(1.0f)
  {}

  explicit Quaternion(const float arDataIn[kNumQuaternionInds])
  {
    memcpy(arData, arDataIn, sizeof(arData));
  }

  Quaternion(float i_, float j_, float k_, float w_)
  : i(i_),
    j(j_),
    k(k_),
    w(w_)
  {}

  // general
  /// Calculates the conjugate of this quaternion.
  void  Conjugate()
  {
    i = -i;
    j = -j;
    k = -k;
  }

  ///@brief Calculates the magnitude of this quaternion.
  float Magnitude() const
  {
    return sqrtf(w * w + i * i + j * j + k * k);
  }

  ///@brief Scales this quaternion by the inverse of the magnitude, i.e.
  /// makes its magnitude 1.0f.
  ///@note Quaternion must be non zero.
  void  Normalise()
  {
    float mag = Magnitude();
    XR_ASSERT(Quaternion, mag > .0f);

    mag = 1.0f / mag;
    i *= mag;
    j *= mag;
    k *= mag;
    w *= mag;
  }

  ///@brief Calculates an axis and an angle that describe the rotation
  /// that the quaternion represents, and stores it in the given values.
  void  GetAxisAngle(float& x, float& y, float& z, float& theta) const
  {
    theta = 2.0f * acosf(w);
    float w2 = w * w;
    if (w2 < 1.0f)
    {
      w2 = 1.0f / (1.0f - sqrtf(w2));
      x = i * w2;
      y = j * w2;
      z = k * w2;
    }
    else
    {
      x = .0f;
      y = .0f;
      z = .0f;
    }
  }

  ///@brief Calculates an axis and an angle that describe the rotation
  /// that the quaternion represents, and stores it in the given values.
  void  GetAxisAngle(Vector3& v, float& theta) const
  {
    GetAxisAngle(v.x, v.y, v.z, theta);
  }

  ///@brief Rotates the vector @a v around the origin by this quaternion.
  void  RotateVec(Vector3& v) const
  {
    const float px((w * v.x) - (j * v.z) + (k * v.y));
    const float py((w * v.y) - (k * v.x) + (i * v.z));
    const float pz((w * v.z) - (i * v.y) + (j * v.x));

    const float wpx(w * px);
    const float wpy(w * py);
    const float wpz(w * pz);

    const float vxpx((j * pz) - (k * py));
    const float vxpy((k * px) - (i * pz));
    const float vxpz((i * py) - (j * px));

    const float vxi2(v.x * i * i);
    const float vyj2(v.y * j * j);
    const float vzk2(v.z * k * k);

    v.x = vxi2 + wpx - vxpx;
    v.y = vyj2 + wpy - vxpy;
    v.z = vzk2 + wpz - vxpz;
  }

  ///@brief Rotates the vector @a v around the given pivot by this quaternion.
  void  RotateVec(Vector3 const& pivot, Vector3& v)
  {
    v -= pivot;
    RotateVec(v);
    v += pivot;
  }

  ///@brief Creates a vector containing the euler angles of the rotation that
  /// this quaternion represents.
  Vector3 ToEuler() const
  {
    const float iSqr(i * i);
    const float jSqr(j * j);
    const float kSqr(k * k);

    Vector3 v(asinf(-2.0f * (i * k - j * w)),
      atan2f(2.0f * (k * j + i * w), 1.0f - 2.0f * (iSqr + jSqr)),
      atan2f(2.0f * (i * j + k * w), 1.0f - 2.0f * (jSqr + kSqr)));
    return v;
  }

  // operator overloads
  Quaternion  operator-() const
  {
    Quaternion  temp(*this);
    temp.Conjugate();
    return temp;
  }

  Quaternion& operator*=(Quaternion const& rhs)
  {
    const float w0(w);
    const float i0(i);
    const float j0(j);
    const float k0(k);

    const float w1(rhs.w);
    const float i1(rhs.i);
    const float j1(rhs.j);
    const float k1(rhs.k);

    w = w0 * w1 - i0 * i1 - j0 * j1 - k0 * k1;
    i = w0 * i1 + i0 * w1 + j0 * k1 - k0 * j1;
    j = w0 * j1 - i0 * k1 + j0 * w1 + k0 * i1;
    k = w0 * k1 + i0 * j1 - j0 * i1 + k0 * w1;
    return *this;
  }

  Quaternion  operator*(Quaternion const& rhs) const
  {
    Quaternion temp(*this);
    return temp *= rhs;
  }

  Quaternion& operator*=(float s)
  {
    w *= s;
    i *= s;
    j *= s;
    k *= s;
    return *this;
  }

  Quaternion operator*(float s) const
  {
    Quaternion temp(*this);
    return temp *= s;
  }

  operator Matrix() const
  {
    const float ij(2.0f * i * j);
    const float wk(2.0f * w * k);
    const float ik(2.0f * i * k);
    const float wj(2.0f * w * j);
    const float jk(2.0f * j * k);
    const float wi(2.0f * w * i);

    const float wSqr(w * w);
    const float iSqr(i * i);
    const float jSqr(j * j);
    const float kSqr(k * k);

    const float arMatrix[Matrix::kNumLinearComponents] =
    {
      wSqr + iSqr - jSqr - kSqr, ij - wk, ik + wj,
      ij + wk, wSqr - iSqr + jSqr - kSqr, jk - wi,
      ik - wj, jk + wi, wSqr - iSqr - jSqr + kSqr
    };
    return Matrix(arMatrix);
  }
};

}

#define XR_TRACE_QUAT(chnl, quat)\
{\
  XR::Quaternion const& q = (quat);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f, %.3f }", #quat, q.i, q.j, q.k, q.w));\
}\

#endif //XR_QUATERNION_HPP
