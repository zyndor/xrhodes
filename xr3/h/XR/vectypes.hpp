//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_VECTYPES_HPP
#define XR_VECTYPES_HPP

#include "debug.hpp"
#include "maths.hpp"
#include <cstring>
#include <algorithm>

namespace XR
{

//==============================================================================
enum  VectorInds
{
  VX,
  VY,
  kNumVector2Inds,
  VZ = kNumVector2Inds,
  kNumVector3Inds,
  VW = kNumVector3Inds, 
  kNumVector4Inds
};

//==============================================================================
struct SVector2 
{
  // static
  static SVector2  s_zero;
  
  // data
  union
  {
    struct
    {
      int16_t x, y;
    };
    int16_t arData[kNumVector2Inds];
  };

  // structors
  SVector2()
  : x(0), y(0)
  {}
  
  SVector2(int16_t x_, int16_t y_)
  : x(x_), y(y_)
  {}

  explicit SVector2(const int16_t parData[2])
  : x(parData[0]),
    y(parData[1])
  {}

  // general
  int16_t Magnitude() const
  {
    return int16_t(sqrtf(Dot()));
  }

  int16_t Dot() const
  {
    return Dot(*this);
  }

  int16_t Dot(const SVector2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }

  // operators
  SVector2& operator +=(const SVector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  SVector2  operator +(const SVector2& rhs) const
  {
    SVector2  temp(rhs);
    temp += *this;
    return temp;
  }

  SVector2& operator -=(const SVector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  SVector2  operator -(const SVector2& rhs) const
  {
    SVector2  temp(rhs);
    temp -= *this;
    return temp;
  }

  SVector2 operator -() const
  {
    return SVector2(-x, -y);
  }
};

//==============================================================================
struct Vector2 
{
  // static
  static Vector2  s_zero;

  // data
  union
  {
    struct
    {
      float x, y;
    };
    float arData[kNumVector2Inds];
  };

  // structors
  Vector2()
  {}

  Vector2(float x_, float y_)
  : x(x_), y(y_)
  {}

  explicit Vector2(const float parData[2])
  : x(parData[0]),
    y(parData[1])
  {}

  // general
  ///@brief Calculates the magnitude of this vector.
  float Magnitude() const
  {
    return sqrtf(Dot());
  }

  ///@brief Calculates the dot product of this vector with itself.
  float Dot() const
  {
    return Dot(*this);
  }

  ///@brief Calculates the dot product of this vector with @a rhs.
  float Dot(const Vector2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }

  ///@brief Normalises this vector.
  Vector2&  Normalise(float s = 1.0f)
  {
    float d(Dot(*this));
    XR_ASSERT(Vector2, d > .0f);
    return this->operator*=(s / sqrtf(d));
  }

  ///@brief Pseudo-cross product - calculates the dot product of this
  /// vector with the perpendicular of @a rhs.
  float Cross(const Vector2& rhs) const
  {
    return x * rhs.y - y * rhs.x;
  }
  
  ///@brief Linearly interpolates between this vector and @a to, at the given @a t blend factor.
  Vector2 Lerp(const Vector2& to, float t) const
  {
    return Vector2(x + (to.x - x) * t, y + (to.y - y) * t);
  }

  ///@brief Returns a conversion of this vector into polar coordinates.
  Vector2 ToPolar() const
  {
    return Vector2(atan2f(y, x), Magnitude());
  }

  ///@brief Returns a conversion of this vector into cartesian coordinates.
  Vector2 ToCartesian() const
  {
    return Vector2(cosf(x) * y, sinf(x) * y);
  }

  // operators
  Vector2& operator +=(const Vector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  Vector2  operator +(const Vector2& rhs) const
  {
    return Vector2(x + rhs.x, y + rhs.y);
  }

  Vector2& operator -=(const Vector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  Vector2  operator -(const Vector2& rhs) const
  {
    return Vector2(x - rhs.x, y - rhs.y);
  }

  Vector2 operator -() const
  {
    return Vector2(-x, -y);
  }
  
  Vector2&  operator *=(float s)
  {
    x *= s;
    y *= s;
    return *this;
  }

  Vector2&  operator /=(float s)
  {
    XR_ASSERT(Vector2, s != .0f);
    s = 1.0f / s;
    x *= s;
    y *= s;
    return *this;
  }

  Vector2  operator *(float s) const
  {
    return Vector2(x * s, y * s);
  }

  Vector2  operator /(float s) const
  {
    XR_ASSERT(Vector2, s != .0f);
    s = 1.0f / s;
    return Vector2(x * s, y * s);
  }

  Vector2&  operator*=(Vector2 const& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
  }

  Vector2 operator*(Vector2 const& rhs) const
  {
    Vector2 temp(*this);
    temp *= rhs;
    return temp;
  }

  Vector2&  operator/=(Vector2 const& rhs)
  {
    XR_ASSERT(Vector2, rhs.x != .0f);
    x /= rhs.x;
    XR_ASSERT(Vector2, rhs.y != .0f);
    y /= rhs.y;
    return *this;
  }

  Vector2 operator/(Vector2 const& rhs) const
  {
    Vector2 temp(*this);
    temp /= rhs;
    return temp;
  }
};

//==============================================================================
struct Vector3 
{
  // static
  static Vector3  s_zero;
  static Vector3  s_one;
  static Vector3  s_xAxis;
  static Vector3  s_yAxis;
  static Vector3  s_zAxis;

  // data
  union
  {
    struct 
    {
      float  x, y, z;
    };
    float  arData[kNumVector3Inds];
  };

  // structors
  Vector3()
  : x(.0f), y(.0f), z(.0f)
  {}
  
  explicit Vector3(const float arData[3])
  : x(arData[VX]),
    y(arData[VY]),
    z(arData[VZ])
  {}
  
  Vector3(float x_, float y_, float z_)
  : x(x_),
    y(y_),
    z(z_)
  {}

  // general
  ///@brief Calculates the magnitude of this vector.
  float Magnitude() const
  {
    return sqrtf(Dot());
  }

  ///@brief Calculates the dot product of this vector with itself.
  float Dot() const
  {
    return Dot(*this);
  }

  ///@brief Calculates the dot product of this vector with @a rhs.
  float Dot(const Vector3& rhs) const
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  ///@brief Normalises this vector.
  Vector3&  Normalise(float s = 1.0f)
  {
    float d(Dot());
    XR_ASSERT(Vector3, d > .0f);
    return this->operator*=(s / sqrtf(d));
  }

  ///@brief Calculates the cross product of this vector with @a rhs.
  Vector3  Cross(const Vector3& rhs) const
  {
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y -
      y * rhs.x);
  }

  ///@brief Linearly interpolates between this vector and @a to, at the given @a t blend factor.
  Vector3 Lerp(const Vector3& to, float t) const
  {
    return Vector3(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
  }

  ///@brief Provides a Vector2 using the x and y components of this vector.
  Vector2 XY() const
  {
    return Vector2(x, y);
  }

  ///@brief Provides a Vector2 using the y and z components of this vector.
  Vector2 YZ() const
  {
    return Vector2(y, z);
  }

  ///@brief Provides a Vector2 using the z and x components of this vector.
  Vector2 ZX() const
  {
    return Vector2(z, x);
  }

  // operators
  Vector3& operator +=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vector3  operator +(const Vector3& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  Vector3& operator -=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  Vector3  operator -(const Vector3& rhs) const
  {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
  }
  
  Vector3 operator -() const
  {
    return Vector3(-x, -y, -z);
  }

  Vector3&  operator *=(float s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  Vector3&  operator /=(float s)
  {
    XR_ASSERT(Vector3, s != .0f);
    s = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  Vector3  operator *(float s) const
  {
    return Vector3(x * s, y * s, z * s);
  }

  Vector3  operator /(float s) const
  {
    XR_ASSERT(Vector3, s != .0f);
    s = 1.0f / s;
    return Vector3(x * s, y * s, z * s);
  }

  Vector3&  operator*=(Vector3 const& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
  }

  Vector3 operator*(Vector3 const& rhs) const
  {
    Vector3 temp(*this);
    temp *= rhs;
    return temp;
  }

  Vector3&  operator/=(Vector3 const& rhs)
  {
    XR_ASSERT(Vector3, rhs.x != .0f);
    x /= rhs.x;
    XR_ASSERT(Vector3, rhs.y != .0f);
    y /= rhs.y;
    XR_ASSERT(Vector3, rhs.z != .0f);
    z /= rhs.z;
    return *this;
  }

  Vector3 operator/(Vector3 const& rhs) const
  {
    Vector3 temp(*this);
    temp /= rhs;
    return temp;
  }
};

//==============================================================================
enum  MatrixInds
{
  // components of x axis - right
  MXX,
  MXY,
  MXZ,
  // components of y axis - up
  MYX,
  MYY,
  MYZ,
  // components of z axis - forward
  MZX,
  MZY,
  MZZ,
  kNumMatrixInds
};

//==============================================================================
///@brief Row-order matrix with a 3x3 linear transformation (rotation, scaling)
/// and 1x3 translation components.
struct Matrix 
{
  // static
  static const float  karIdentityData[kNumMatrixInds];
  
  static Matrix s_identity;
  
  // data
  union
  {
    float  arLinear[kNumMatrixInds];
    float  arLinear2D[kNumVector3Inds][kNumVector3Inds];
    struct
    {
      float  xx, xy, xz, // local x axis
              yx, yy, yz, // local y axis
              zx, zy, zz; // local z axis
    };
  };
  Vector3 t;
  
  // structors
  Matrix()
  {}

  explicit Matrix(const Vector3& t_)
  : t(t_)
  {
    memcpy(arLinear, karIdentityData, sizeof(arLinear));
  }

  explicit Matrix(const float arData[kNumMatrixInds])
  : t()
  {
    CopyLinear(arData);
  }

  Matrix(const Matrix& rhs, const Vector3& t_)
  : t(t_)
  {
    CopyLinear(rhs);
  }

  // general
  Vector3 GetColumn(VectorInds i) const
  {
    XR_ASSERT(Matrix, i < kNumVector3Inds);
    return Vector3(arLinear[i], arLinear[i + kNumVector3Inds], arLinear[i + 2 * kNumVector3Inds]);
  }

  void SetColumn(VectorInds i, Vector3 const& v)
  {
    XR_ASSERT(Matrix, i < kNumVector3Inds);
    arLinear[i] = v.x;
    arLinear[i + kNumVector3Inds] = v.y;
    arLinear[i + 2 * kNumVector3Inds] = v.z;
  }

  void  ScaleX(float s)
  {
    xx *= s;
    xy *= s;
    xz *= s;
  }
  
  void  ScaleY(float s)
  {
    yx *= s;
    yy *= s;
    yz *= s;
  }
  
  void  ScaleZ(float s)
  {
    zx *= s;
    zy *= s;
    zz *= s;
  }
  
  ///@brief Scales the rotation part of this matrix by the scalar @a s.
  void  ScaleRot(float s)
  {
    xx *= s;
    xy *= s;
    xz *= s;
    yx *= s;
    yy *= s;
    yz *= s;
    zx *= s;
    zy *= s;
    zz *= s;
  }

  ///@brief Sets the matrix to represent a rotation around the X axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the 
  /// rotation, to zero (@a setZeros).
  void  SetRotX(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::s_zero;
    }
    
    if (setZeros)
    {
      xy = .0f;
      xz = .0f;
      yx = .0f;
      zx = .0f;
    }

    xx = 1.0f;
    yy = zz = cosf(theta);
    yz = -(zy = sinf(theta));
  }
  
  ///@brief Sets the matrix to represent a rotation around the Y axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the 
  /// rotation, to zero (@a setZeros).
  void  SetRotY(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::s_zero;
    }

    if (setZeros)
    {
      xy = .0f;
      yz = .0f;
      yx = .0f;
      zy = .0f;
    }

    yy = 1.0f;
    xx = zz = cosf(theta);
    zx = -(xz = sinf(theta));
  }

  ///@brief Sets the matrix to represent a rotation around the Z axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the 
  /// rotation, to zero (@a setZeros).
  void  SetRotZ(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::s_zero;
    }

    if (setZeros)
    {
      xz = .0f;
      yz = .0f;
      zx = .0f;
      zy = .0f;
    }

    zz = 1.0f;
    xx = yy = cosf(theta);
    xy = -(yx = sinf(theta));
  }
  
  ///@brief Transform the rotation part of the matrix only, by
  /// rotating it @a theta degrees around the X axis.
  void  RotateX(float theta)
  {
    Matrix  m;
    m.SetRotX(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the rotation part of the matrix only, by
  /// rotating it @a theta degrees around the Y axis.
  void  RotateY(float theta)
  {
    Matrix  m;
    m.SetRotY(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the rotation part of the matrix only, by
  /// rotating it @a theta degrees around the Z axis.
  void  RotateZ(float theta)
  {
    Matrix  m;
    m.SetRotZ(theta, true, true);
    RotateBy(m);
  }

  ///@brief Calculates the amount of rotation the matrix represents around the
  /// X axis.
  float DirX() const
  {
    Vector3 v(RotateVec(Vector3::s_xAxis));
    return  atan2f(v.y, v.z);
  }
  
  ///@brief Calculates the amount of rotation the matrix represents around the
  /// Y axis.
  float DirY() const
  {
    Vector3 v(RotateVec(Vector3::s_yAxis));
    return  atan2f(v.x, v.z);
  }
  
  ///@brief Calculates the amount of rotation the matrix represents around the
  /// Z axis.
  float DirZ() const
  {
    Vector3 v(RotateVec(Vector3::s_zAxis));
    return  atan2f(v.y, v.x);
  }
  
  ///@brief Copies the linear transformation part from the given array.
  void  CopyLinear(const float parLinear[kNumMatrixInds])
  {
    memcpy(arLinear, parLinear, sizeof(arLinear));
  }
  
  ///@brief Copies the linear transformation part of the given matrix.
  void  CopyLinear(const Matrix& m)
  {
    CopyLinear(m.arLinear);
  }
  
  ///@brief Multiplies the linear transformation component of this Matrix
  /// by the linear transformation component the other Matrix @a m.
  /// No translation applied or modified.
  void  RotateBy(const Matrix& m)
  {
    Matrix  n(*this);
    CalculateComponentProduct(m, n, VX, VX);
    CalculateComponentProduct(m, n, VX, VY);
    CalculateComponentProduct(m, n, VX, VZ);
    CalculateComponentProduct(m, n, VY, VX);
    CalculateComponentProduct(m, n, VY, VY);
    CalculateComponentProduct(m, n, VY, VZ);
    CalculateComponentProduct(m, n, VZ, VX);
    CalculateComponentProduct(m, n, VZ, VY);
    CalculateComponentProduct(m, n, VZ, VZ);
  }
  
  void  CalculateComponentProduct(const Matrix& m, const Matrix& n, int i, int j)
  {
    XR_ASSERT(Matrix, i >= 0);
    XR_ASSERT(Matrix, i < kNumVector3Inds);
    XR_ASSERT(Matrix, j >= 0);
    XR_ASSERT(Matrix, j < kNumVector3Inds);

    int y(j * kNumVector3Inds);
    arLinear[y + i] = m.arLinear[y] * n.arLinear[i] +
      m.arLinear[y + 1] * n.arLinear[i + kNumVector3Inds] +
      m.arLinear[y + 2] * n.arLinear[i + kNumVector3Inds * 2];
  }

  ///@brief Transforms this matrix by the matrix @a m, applying its
  /// rotation and translation.
  void  TransformBy(const Matrix& m)
  {
    RotateBy(m);
    t = m.TransformVec(t);
  }

  ///@brief Rotates the vector by this matrix.
  Vector3  RotateVec(const Vector3& v) const
  {
    return Vector3(v.x * xx + v.y * yx + v.z * zx,
      v.x * xy + v.y * yy + v.z * zy,
      v.x * xz + v.y * yz + v.z * zz);
  }
  
  ///@brief Transforms the vector by this matrix.
  Vector3  TransformVec(const Vector3& v) const
  {
    return RotateVec(v) + t;
  }
  
  ///@brief Calculates a transformation looking from the position @a from to
  /// @a to, with the given @a up vector.
  void  LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
  {
    Vector3  vz(to - from);
    float dz(vz.Dot(vz));
    if (dz > .0f)
    {
      dz = 1.0f / sqrtf(dz);
      vz *= dz;
    }
    else
    {
      vz.x = 1.0f;
    }

    Vector3  vx(up.Cross(vz));  // local x axis
    Vector3  vy(vz.Cross(vx));  // local y axis
    memcpy(arLinear + MXX, vx.arData, sizeof(vx));
    memcpy(arLinear + MYX, vy.arData, sizeof(vy));
    memcpy(arLinear + MZX, vz.arData, sizeof(vz));
  }

  ///@brief Calculates a transformation looking from the translation part
  /// of the matrix used as a position, to @a to, with the given @a up vector.
  void  LookAt(const Vector3& to, const Vector3& up)
  {
    LookAt(Vector3(t.x, t.y, t.z), to, up);
  }

  ///@brief Writes data from the matrix into the given array in 
  /// OpenGL's 4x4 column-order format.
  void  ToGL(float arDataOut[16]) const
  {
    XR_ASSERT(Matrix, arDataOut != 0);
    arDataOut[0] = xx;
    arDataOut[1] = yx;
    arDataOut[2] = zx;
    arDataOut[3] = .0f;
    arDataOut[4] = xy;
    arDataOut[5] = yy;
    arDataOut[6] = zy;
    arDataOut[7] = .0f;
    arDataOut[8] = xz;
    arDataOut[9] = yz;
    arDataOut[10] = zz;
    arDataOut[11] = .0f;
    arDataOut[12] = t.x;
    arDataOut[13] = t.y;
    arDataOut[14] = t.z;
    arDataOut[15] = 1.0f;
  }
  
  ///@brief Changes the linear transformation part of this matrix to its transpose.
  Matrix&  Transpose()
  {
    std::swap(arLinear[MXY], arLinear[MYX]);
    std::swap(arLinear[MZX], arLinear[MXZ]);
    std::swap(arLinear[MYZ], arLinear[MZY]);
    return *this;
  }
  
  ///@brief Calculates the transpose of the rotation part of this matrix.
  Matrix  Transposed() const
  {
    return Matrix(*this).Transpose();
  }

  // operator overloads
  Matrix&  operator*=(const Matrix& rhs)
  {
    TransformBy(rhs);
    return *this;
  }

  Matrix  operator*(const Matrix& rhs) const
  {
    Matrix  product(*this);
    return product *= rhs;
  }
};

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
  static const float  karUnitQuaternionData[kNumQuaternionInds];
  
  static Quaternion s_unit;

  ///@brief Creates a quaternion from a vector of pitch, yaw, roll rotations.
  ///@note The angles are in radians.
  static void CreateFromEuler(Vector3 pitchYawRoll, Quaternion& q)
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

  static Quaternion CreateFromEuler(Vector3 const& pitchYawRoll)
  {
    Quaternion  q;
    CreateFromEuler(pitchYawRoll, q);
    return q;
  }

  ///@brief Creates a quaternion that rotates by @a angle radians around the
  /// given @axis.
  ///@note The angle needs to be in radians.
  static void CreateFromAxisAngle(Vector3 const& axis, float angle, Quaternion& q)
  {
    angle *= .5f;
    q.w = cosf(angle);

    const float s = sinf(angle);
    q.i = axis.x * s;
    q.j = axis.y * s;
    q.k = axis.z * s;
    q.Normalise();
  }

  static Quaternion CreateFromAxisAngle(Vector3 const& axis, float angle)
  {
    Quaternion  q;
    CreateFromAxisAngle(axis, angle, q);
    return q;
  }

  ///@brief Offers more efficient creation of a quaternion that rotates from
  /// @a from to @a to. The vectors must be normalised.
  static void CreateFromToDirectionNormalised(Vector3 const& from, Vector3 const& to, Quaternion& q)
  {
    float d = from.Dot(to);
    if (d * d < 1.0f)
    {
      Vector3 c = from.Cross(to);
      Quaternion q(c.x, c.y, c.z, d + 1.0f);
      q.Normalise();
    }
  }

  static Quaternion CreateFromToDirectionNormalised(Vector3 const& from, Vector3 const& to)
  {
    Quaternion  q;
    CreateFromToDirectionNormalised(from, to, q);
    return q;
  }

  ///@brief Creates a quaternion that rotates from @a from to @a to. These
  /// vectors will be normalised.
  static void CreateFromToDirection(Vector3 from, Vector3 to, Quaternion& q)
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

    CreateFromToDirectionNormalised(from, to, q);
  }

  static Quaternion CreateFromToDirection(Vector3 const& from, Vector3 const& to)
  {
    Quaternion  q;
    CreateFromToDirection(from, to, q);
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
  {
    memcpy(arData, karUnitQuaternionData, sizeof(arData));
  }

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
    const float wSqr(w * w);

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

  Quaternion& operator*=(const Quaternion& rhs)
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
  
  Quaternion  operator*(const Quaternion& rhs) const
  {
    Quaternion temp(*this);
    return temp *= rhs;
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

    const float arMatrix[kNumMatrixInds] =
    {
      wSqr + iSqr - jSqr - kSqr, ij - wk, ik + wj,
      ij + wk, wSqr - iSqr + jSqr - kSqr, jk - wi,
      ik - wj, jk + wi, wSqr - iSqr - jSqr + kSqr
    };
    return Matrix(arMatrix);
  }
};

} // XR

#define XR_TRACE_VECTOR2(name, vector)  XR_TRACE(name, ("{ %.3f, %.3f }",\
  name, vector.x, vector.y))
#define XR_TRACE_VECTOR3(name, vector)  XR_TRACE(name, ("{ %.3f, %.3f, %.3f }",\
  name, vector.x, vector.y, vector.z))
#define XR_TRACE_MATRIX(name, matrix)  XR_TRACE(name, ("{ %.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f } }",\
  name, matrix.xx, matrix.xy, matrix.xz, matrix.yx, matrix.yy, matrix.yz, matrix.zx, matrix.zy, matrix.zz))
#define XR_TRACE_QUAT(name, quat)  XR_TRACE(name, ("{ %.3f, %.3f, %.3f, %.3f }",\
  name, quat.i, quat.j, quat.k, quat.w))


#endif // XR_VECTYPES_HPP