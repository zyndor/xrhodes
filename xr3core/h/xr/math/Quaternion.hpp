#ifndef XR_QUATERNION_HPP
#define XR_QUATERNION_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Matrix.hpp"
#include "xr/math/Vector3.hpp"
#include "xr/debug.hpp"
#include <cstring>
#include <cstdint>
#include <cmath>

namespace xr
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
  inline
  static constexpr Quaternion Identity()
  {
    return Quaternion(0.f, 0.f, 0.f, 1.f);
  }

  ///@brief Creates a quaternion from a vector of pitch, yaw, roll rotations.
  ///@note The angles are in radians.
  static void FromPitchYawRoll(float pitch, float yaw, float roll, Quaternion& q)
  {
    pitch *= .5f;
    yaw *= .5f;
    roll *= .5f;

    const float c1 = std::cos(yaw);
    const float c2 = std::cos(pitch);
    const float c3 = std::cos(roll);
    const float s1 = std::sin(yaw);
    const float s2 = std::sin(pitch);
    const float s3 = std::sin(roll);

    const float c2c3 = c2 * c3;
    const float s2s3 = s2 * s3;
    const float s2c3 = s2 * c3;
    const float c2s3 = c2 * s3;

    q.i = s1 * c2c3 - c1 * s2s3;
    q.j = c1 * s2c3 - s1 * c2s3;
    q.k = c1 * c2s3 - s1 * s2c3;
    q.w = c1 * c2c3 - s1 * s2s3;
  }

  static void FromPitchYawRoll(Vector3 const& pitchYawRoll, Quaternion& q)
  {
    FromPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z, q);
  }

  static Quaternion FromPitchYawRoll(float pitch, float yaw, float roll)
  {
    Quaternion  q;
    FromPitchYawRoll(pitch, yaw, roll, q);
    return q;
  }

  static Quaternion FromPitchYawRoll(Vector3 const& pitchYawRoll)
  {
    Quaternion  q;
    FromPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z, q);
    return q;
  }

  ///@brief Creates a quaternion that rotates by @a angle radians around the
  /// given @axis.
  ///@note The angle needs to be in radians.
  static void FromAxisAngle(Vector3 const& axis, float angle, Quaternion& q)
  {
    angle *= .5f;
    q.w = std::cos(angle);

    const float s = std::sin(angle);
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
    if (d * d < 1.f)
    {
      Vector3 c = from.Cross(to);
      q = Quaternion(c.x, c.y, c.z, d + 1.f);
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
    if (dFrom > 0.f)
    {
      from *= 1.f / std::sqrt(dFrom);
    }

    float dTo = to.Dot();
    if (dTo > 0.f)
    {
      to *= 1.f / std::sqrt(dTo);
    }

    FromPositionsNormalised(from, to, q);
  }

  static Quaternion FromPositions(Vector3 const& from, Vector3 const& to)
  {
    Quaternion  q;
    FromPositions(from, to, q);
    return q;
  }

  ///@brief Creates a quaternion which describes the same rotation as the matrix @a m.
  static Quaternion FromMatrix(Matrix const& m)
  {
    float trace = m.xx + m.yy + m.zz;
    if (trace > 0.f)
    {
      const float s = .5f / std::sqrt(trace + 1.f);
      return Quaternion((m.zy - m.yz) * s, (m.xz - m.zx) * s,
        (m.yx - m.xy) * s, .25f / s);
    }
    else if (m.xx > m.yy && m.xx > m.zz)
    {
      const float s = .5f / std::sqrt(1.f + m.xx - m.yy - m.zz);
      return Quaternion(.25f / s, (m.xy + m.yx) * s, (m.xz + m.zx) * s,
        (m.zy - m.yz) * s);
    }
    else if (m.yy > m.zz)
    {
      const float s = .5f / std::sqrt(1.f + m.yy - m.xx - m.zz);
      return Quaternion((m.xy + m.yx) * s, .25f / s, (m.yz + m.zy) * s,
        (m.xz - m.zx) * s);
    }
    else
    {
      const float s = .5f / std::sqrt(1.f + m.zz - m.xx - m.yy);
      return Quaternion((m.xz + m.zx) * s, (m.yz + m.zy) * s, .25f / s,
        (m.yx - m.xy) * s);
    }
  }

  // data
  union
  {
    struct
    {
      float i, j, k, w;
    };
    float data[kNumQuaternionInds];
  };

  // structors
  constexpr Quaternion()
  : Quaternion(0.f, 0.f, 0.f, 1.f)
  {}

  explicit constexpr Quaternion(const float data[kNumQuaternionInds])
  : i{ data[QI] }, j{ data[QJ] }, k{ data[QK] }, w{ data[QW] }
  {}

  constexpr Quaternion(float i_, float j_, float k_, float w_)
  : i{ i_ },
    j{ j_ },
    k{ k_ },
    w{ w_ }
  {}

  // general
  ///@brief Calculates the conjugate of this quaternion.
  ///@note For a unit quaternion, this is also its inverse.
  void  Conjugate()
  {
    i = -i;
    j = -j;
    k = -k;
  }

  ///@brief Calculates the magnitude of this quaternion.
  float Magnitude() const
  {
    return std::sqrt(w * w + i * i + j * j + k * k);
  }

  ///@brief Normalises this quaternion so that its magnitude is 1.
  ///@note Quaternion must be non zero.
  void  Normalise()
  {
    float mag = Magnitude();
    XR_ASSERT(Quaternion, mag > 0.f);

    mag = 1.f / mag;
    i *= mag;
    j *= mag;
    k *= mag;
    w *= mag;
  }

  ///@return A copy of this quaternion, scaled so that its magnitude is 1.
  ///@note Quaternion must be non zero.
  Quaternion Normalised() const
  {
    Quaternion q(*this);
    q.Normalise();
    return q;
  }

  ///@brief Calculates an axis and an angle that describe the rotation
  /// that the quaternion represents, and stores it in the given values.
  void  GetAxisAngle(float& x, float& y, float& z, float& theta) const
  {
    theta = 2.f * std::acos(w);
    float w2 = w * w;
    if (w2 < 1.f)
    {
      w2 = 1.f / (1.f - std::sqrt(w2));
      x = i * w2;
      y = j * w2;
      z = k * w2;
    }
    else
    {
      x = 0.f;
      y = 0.f;
      z = 0.f;
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
    const float px = (w * v.x) - (j * v.z) + (k * v.y);
    const float py = (w * v.y) - (k * v.x) + (i * v.z);
    const float pz = (w * v.z) - (i * v.y) + (j * v.x);

    const float wpx = w * px;
    const float wpy = w * py;
    const float wpz = w * pz;

    const float vxpx = (j * pz) - (k * py);
    const float vxpy = (k * px) - (i * pz);
    const float vxpz = (i * py) - (j * px);

    const float vxi2 = v.x * i * i;
    const float vyj2 = v.y * j * j;
    const float vzk2 = v.z * k * k;

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
    const float iSqr = i * i;
    const float jSqr = j * j;
    const float kSqr = k * k;

    Vector3 v(std::asin(-2.f * (i * k - j * w)),
      atan2f(2.f * (k * j + i * w), 1.f - 2.f * (iSqr + jSqr)),
      atan2f(2.f * (i * j + k * w), 1.f - 2.f * (jSqr + kSqr)));
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
    const float i0 = i;
    const float j0 = j;
    const float k0 = k;
    const float w0 = w;

    const float i1 = rhs.i;
    const float j1 = rhs.j;
    const float k1 = rhs.k;
    const float w1 = rhs.w;

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
    i *= s;
    j *= s;
    k *= s;
    w *= s;
    return *this;
  }

  Quaternion operator*(float s) const
  {
    Quaternion temp(*this);
    return temp *= s;
  }

  Quaternion& operator+=(Quaternion const& rhs)
  {
    i += rhs.i;
    j += rhs.j;
    k += rhs.k;
    w += rhs.w;
    return *this;
  }

  Quaternion operator+(Quaternion const& rhs) const
  {
    Quaternion temp(*this);
    return temp += rhs;
  }

  operator Matrix() const
  {
    const float ij = 2.f * i * j;
    const float wk = 2.f * w * k;
    const float ik = 2.f * i * k;
    const float wj = 2.f * w * j;
    const float jk = 2.f * j * k;
    const float wi = 2.f * w * i;

    const float iSqr = i * i;
    const float jSqr = j * j;
    const float kSqr = k * k;
    const float wSqr = w * w;

    const float matrixData[Matrix::kNumLinearComponents] =
    {
      wSqr + iSqr - jSqr - kSqr, ij - wk, ik + wj,
      ij + wk, wSqr - iSqr + jSqr - kSqr, jk - wi,
      ik - wj, jk + wi, wSqr - iSqr - jSqr + kSqr
    };
    return Matrix(matrixData);
  }
};

}

#define XR_TRACE_QUAT(chnl, quat)\
{\
  xr::Quaternion const& q = (quat);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f, %.3f }", #quat, q.i, q.j, q.k, q.w));\
}\

#endif //XR_QUATERNION_HPP
