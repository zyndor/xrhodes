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
#include "xr/math/Matrix.hpp"
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
///@brief Represents a rotation in 3D space.
class Quaternion
{
public:
  // static
  inline
  static constexpr Quaternion Identity()
  {
    return Quaternion(0.f, 0.f, 0.f, 1.f);
  }

  ///@brief Creates a quaternion from pitch (x), yaw (y) and roll (z) rotations.
  ///@note The angles are in radians.
  static void FromPitchYawRoll(float pitch, float yaw, float roll, Quaternion& q)
  {
    pitch *= .5f;
    yaw *= .5f;
    roll *= .5f;

    const float cPitch = std::cos(pitch);
    const float cYaw = std::cos(yaw);
    const float cRoll = std::cos(roll);
    const float sPitch = std::sin(pitch);
    const float sYaw = std::sin(yaw);
    const float sRoll = std::sin(roll);

    const float cPitch_cRoll = cPitch * cRoll;
    const float cPitch_sRoll = cPitch * sRoll;
    const float sPitch_cRoll = sPitch * cRoll;
    const float sPitch_sRoll = sPitch * sRoll;

    q.i = sPitch_cRoll * cYaw - cPitch_sRoll * sYaw;
    q.j = cPitch_cRoll * sYaw + sPitch_sRoll * cYaw;
    q.k = cPitch_sRoll * cYaw - sPitch_cRoll * sYaw;
    q.w = cPitch_cRoll * cYaw + sPitch_sRoll * sYaw;
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
  ///@note The angle is in radians.
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
      Vector3 c = to.Cross(from);
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
    float trace = m.xx() + m.yy() + m.zz();
    if (trace > 0.f)
    {
      const float s = .5f / std::sqrt(trace + 1.f);
      return Quaternion((m.zy() - m.yz()) * s, (m.xz() - m.zx()) * s,
        (m.yx() - m.xy()) * s, .25f / s);
    }
    else if (m.xx() > m.yy() && m.xx() > m.zz())
    {
      const float s = .5f / std::sqrt(1.f + m.xx() - m.yy() - m.zz());
      return Quaternion(.25f / s, (m.xy() + m.yx()) * s, (m.xz() + m.zx()) * s,
        (m.zy() - m.yz()) * s);
    }
    else if (m.yy() > m.zz())
    {
      const float s = .5f / std::sqrt(1.f + m.yy() - m.xx() - m.zz());
      return Quaternion((m.xy() + m.yx()) * s, .25f / s, (m.yz() + m.zy()) * s,
        (m.xz() - m.zx()) * s);
    }
    else
    {
      const float s = .5f / std::sqrt(1.f + m.zz() - m.xx() - m.yy());
      return Quaternion((m.xz() + m.zx()) * s, (m.yz() + m.zy()) * s, .25f / s,
        (m.yx() - m.xy()) * s);
    }
  }

  // data
  float i, j, k, w;

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
  Quaternion&  Conjugate()
  {
    i = -i;
    j = -j;
    k = -k;
    return *this;
  }

  ///@brief Calculates the dot product of this and @a other.
  constexpr float Dot(Quaternion const& other) const
  {
    return i * other.i + j * other.j + k * other.k + w * other.w;
  }

  ///@brief Calculates the magnitude of this quaternion.
  float Magnitude() const
  {
    return std::sqrt(Dot(*this));
  }

  ///@brief Normalises this quaternion so that its magnitude is 1.
  ///@note Quaternion must be non zero.
  Quaternion& Normalise()
  {
    float mag = Magnitude();
    XR_ASSERT(Quaternion, mag > 0.f);

    mag = 1.f / mag;
    i *= mag;
    j *= mag;
    k *= mag;
    w *= mag;

    return *this;
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
    float wSqr = w * w;
    if (wSqr < 1.f)
    {
      wSqr = 1.f / (1.f - std::sqrt(wSqr));
      x = i * wSqr;
      y = j * wSqr;
      z = k * wSqr;
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
  ///@return The vector @a v rotated by this.
  Vector3 Rotate(Vector3 const& v) const
  {
    auto& q = reinterpret_cast<Vector3 const&>(*this);
    Vector3 r = v * w + v.Cross(q);
    return v + 2.f * r.Cross(q);
  }

  [[deprecated("Use Rotate().")]]
  void  RotateVec(Vector3& v) const
  {
    v = Rotate(v);
  }

  [[deprecated("Use Rotate().")]]
  void  RotateVec(Vector3 const& pivot, Vector3& v) const
  {
    v = Rotate(v - pivot) + pivot;
  }

  ///@return A vector containing the euler angles of the rotation that this quaternion
  /// represents, in pitch, yaw, roll order.
  Vector3 ToEuler() const
  {
    const float iSqr = i * i;
    const float jSqr = j * j;
    const float kSqr = k * k;

    Vector3 v(atan2(2.f * (w * i + k * j), 1.f - 2.f * (iSqr + kSqr)),
      asin(2.f * (w * j - i * k)),
      atan2(2.f * (w * k + i * j), 1.f - 2.f * (jSqr + kSqr)));
    return v;
  }

  // range based for support
  float* begin()
  {
    return &i;
  }

  float* end()
  {
    return &i + kNumQuaternionInds;
  }

  float const* begin() const
  {
    return &i;
  }

  float const* end() const
  {
    return &i + kNumQuaternionInds;
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
