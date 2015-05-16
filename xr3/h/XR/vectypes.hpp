//
// Nuclear Heart Games
// XRhodes
//
// vectypes.hpp
// @author  Gyorgy Straub
// @date    26/09/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_VECTYPES_HPP
#define XR_VECTYPES_HPP

#include <cstring>
#include <algorithm>
#include "types.hpp"
#include "maths.hpp"

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
      int16 x, y;
    };
    int16 arCoord[kNumVector2Inds];
  };

  // structors
  SVector2()
  : x(0), y(0)
  {}
  
  SVector2(int16 x_, int16 y_)
  : x(x_), y(y_)
  {}

  explicit SVector2(const int16 parData[2])
  : x(parData[0]),
    y(parData[1])
  {}

  // general
  int16 Magnitude() const
  {
    return int16(sqrtf(Dot()));
  }

  int16 Dot() const
  {
    return Dot(*this);
  }

  int16 Dot(const SVector2& rhs) const
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
    float arCoord[kNumVector2Inds];
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
  float Magnitude() const
  {
    return sqrtf(Dot());
  }

  float Dot() const
  {
    return Dot(*this);
  }

  float Dot(const Vector2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }

  Vector2&  Normalise(float s = 1.0f)
  {
    float d(Dot(*this));
    XR_ASSERT(Vector2, d > .0f);
    return this->operator*=(s / sqrtf(d));
  }

  float Cross(const Vector2& rhs) const
  {
    return x * rhs.y - y * rhs.x;
  }
  
  Vector2 Lerp(const Vector2& to, float t) const
  {
    return Vector2(x + (to.x - x) * t, y + (to.y - y) * t);
  }

  Vector2 ToPolar() const
  {
    return Vector2(atan2f(y, x), Magnitude());
  }

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
};

//==============================================================================
struct Vector3 
{
  // static
  static Vector3  s_zero;
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
    float  arCoord[kNumVector3Inds];
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
  float Magnitude() const
  {
    return sqrtf(Dot());
  }

  float Dot() const
  {
    return Dot(*this);
  }

  float Dot(const Vector3& rhs) const
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  Vector3&  Normalise(float s = 1.0f)
  {
    float d(Dot(*this));
    XR_ASSERT(Vector3, d > .0f);
    return this->operator*=(s / sqrtf(d));
  }

  Vector3  Cross(const Vector3& rhs) const
  {
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y -
      y * rhs.x);
  }

  Vector3 Lerp(const Vector3& to, float t) const
  {
    return Vector3(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
  }

  Vector2 XY() const
  {
    return Vector2(x, y);
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
struct Matrix 
{
  // static
  static const float  karIdentityData[kNumMatrixInds];
  
  static Matrix s_identity;
  
  // data
  union
  {
    float  arRot[kNumMatrixInds];
    struct
    {
      float  xx, xy, xz, yx, yy, yz, zx, zy, zz;
    };
  };
  Vector3 t;
  
  // structors
  Matrix()
  : t()
  {
    memcpy(arRot, karIdentityData, sizeof(arRot));
  }
  
  explicit Matrix(const Vector3& t_)
  : t(t_)
  {
    memcpy(arRot, karIdentityData, sizeof(arRot));
  }

  explicit Matrix(const float arData[kNumMatrixInds])
  : t()
  {
    memcpy(arRot, arData, sizeof(arRot));
  }

  Matrix(const Matrix& rhs, const Vector3& t_)
  : t(t_)
  {
    memcpy(arRot, rhs.arRot, sizeof(arRot));
  }

  // general
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
  
  void  RotateX(float theta)
  {
    Matrix  m;
    m.SetRotX(theta, true, true);
    RotateBy(m);
  }

  void  RotateY(float theta)
  {
    Matrix  m;
    m.SetRotY(theta, true, true);
    RotateBy(m);
  }

  void  RotateZ(float theta)
  {
    Matrix  m;
    m.SetRotZ(theta, true, true);
    RotateBy(m);
  }
  
  float DirX() const
  {
    Vector3 v(RotateVec(Vector3::s_xAxis));
    return  atan2f(v.y, v.z);
  }
  
  float DirY() const
  {
    Vector3 v(RotateVec(Vector3::s_yAxis));
    return  atan2f(v.x, v.z);
  }
  
  float DirZ() const
  {
    Vector3 v(RotateVec(Vector3::s_zAxis));
    return  atan2f(v.y, v.x);
  }
  
  void  CopyRot(const float parRot[kNumMatrixInds])
  {
    memcpy(arRot, parRot, sizeof(arRot));
  }
  
  void  CopyRot(const Matrix& m)
  {
    CopyRot(m.arRot);
  }
  
  void  RotateBy(const Matrix& m)
  {
    Matrix  n(*this);
    CalculateComponent(m, n, VX, VX);
    CalculateComponent(m, n, VX, VY);
    CalculateComponent(m, n, VX, VZ);
    CalculateComponent(m, n, VY, VX);
    CalculateComponent(m, n, VY, VY);
    CalculateComponent(m, n, VY, VZ);
    CalculateComponent(m, n, VZ, VX);
    CalculateComponent(m, n, VZ, VY);
    CalculateComponent(m, n, VZ, VZ);
  }
  
  Vector3  RotateVec(const Vector3& v) const
  {
    return Vector3(v.x * xx + v.y * yx + v.z * zx,
      v.x * xy + v.y * yy + v.z * zy,
      v.x * xz + v.y * yz + v.z * zz);
  }
  
  Vector3  TransformVec(const Vector3& v) const
  {
    return RotateVec(v) + t;
  }
  
  void  CalculateComponent(const Matrix& m, const Matrix& n, int i, int j)
  {
    XR_ASSERT(Matrix, i >= 0);
    XR_ASSERT(Matrix, i < kNumVector3Inds);
    XR_ASSERT(Matrix, j >= 0);
    XR_ASSERT(Matrix, j < kNumVector3Inds);

    int y(j * kNumVector3Inds);
    arRot[y + i] = m.arRot[y] * n.arRot[i] +
      m.arRot[y + 1] * n.arRot[i + kNumVector3Inds] +
      m.arRot[y + 2] * n.arRot[i + kNumVector3Inds * 2];
  }

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

    Vector3  vx(up.Cross(vz));  // x axis
    Vector3  vy(vz.Cross(vx));  // y axis
    memcpy(arRot + MXX, &vx, sizeof(vx));
    memcpy(arRot + MYX, &vy, sizeof(vy));
    memcpy(arRot + MZX, &vz, sizeof(vz));
  }

  void  LookAt(const Vector3& to, const Vector3& up)
  {
    LookAt(Vector3(t.x, t.y, t.z), to, up);
  }

  void  ToGL(float arData[16]) const
  {
    XR_ASSERT(Matrix, arData != 0);
    arData[0] = xx;
    arData[1] = yx;
    arData[2] = zx;
    arData[3] = .0f;
    arData[4] = xy;
    arData[5] = yy;
    arData[6] = zy;
    arData[7] = .0f;
    arData[8] = xz;
    arData[9] = yz;
    arData[10] = zz;
    arData[11] = .0f;
    arData[12] = t.x;
    arData[13] = t.y;
    arData[14] = t.z;
    arData[15] = 1.0f;
  }
  
  Matrix&  Transpose()
  {
    std::swap(arRot[MXY], arRot[MYX]);
    std::swap(arRot[MZX], arRot[MXZ]);
    std::swap(arRot[MYZ], arRot[MZY]);
    return *this;
  }
  
  Matrix  Transposed() const
  {
    return Matrix(*this).Transpose();
  }

  // operator overloads
  Matrix&  operator*=(const Matrix& rhs)
  {
    RotateBy(rhs);
    t = rhs.TransformVec(t);
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
  
  static Quaternion FromEuler(Vector3 pyr)
  {
    // pitch, yaw, roll
    pyr *= .5f;
  
    const float c1(cosf(pyr.y));
    const float c2(cosf(pyr.x));
    const float c3(cosf(pyr.z));
    const float s1(sinf(pyr.y));
    const float s2(sinf(pyr.x));
    const float s3(sinf(pyr.z));
  
    const float c2c3(c2 * c3);
    const float s2s3(s2 * s3);

    Quaternion  q(s1 * c2c3 - c1 * s2s3,
      c1 * s2 * c3 - s1 * c2 * s3,
      c1 * c2 * s3 - s1 * s2 * c3,
      c1 * c2c3 - s1 * s2s3);
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

  explicit Quaternion(const float arData_[kNumQuaternionInds])
  {
    memcpy(arData, arData_, sizeof(arData));
  }
  
  Quaternion(float i_, float j_, float k_, float w_)
  : i(i_),
    j(j_),
    k(k_),
    w(w_)
  {}
  
  // general
  void  Conjugate()
  {
    i = -i;
    j = -j;
    k = -k;
  }
  
  float Magnitude() const
  {
    return sqrtf(w * w + i * i + j * j + k * k);
  }

  void  Normalise()
  {
    float mag(Magnitude());
    XR_ASSERT(Quaternion, mag > .0f);

    mag = 1.0f / mag;
    i *= mag;
    j *= mag;
    k *= mag;
    w *= mag;
  }
  
  void  SetAxisAngle(float x, float y, float z, float theta)
  {
    theta *= .5f;
    w = cosf(theta);

    const float sinTheta(sinf(theta));
    i = x * sinTheta;
    j = y * sinTheta;
    k = z * sinTheta;
  }
  
  void  SetAxisAngle(const Vector3& v, float theta)
  {
    SetAxisAngle(v.x, v.y, v.z, theta);
  }
  
  void  GetAxisAngle(float& x, float& y, float& z, float& theta)
  {
    theta = acosf(w);

    float sinTheta(sinf(theta));
    if (sinTheta > .0f)
    {
      sinTheta = 1.0f / sinTheta;
      x = i * sinTheta;
      y = j * sinTheta;
      z = k * sinTheta;
    }
    else
    {
      x = y = z = .0f;
    }

    theta *= 2.0f;
  }
  
  void  GetAxisAngle(Vector3& v, float theta)
  {
    GetAxisAngle(v.x, v.y, v.z, theta);
  }
  
  void  RotateVec(Vector3& v)
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