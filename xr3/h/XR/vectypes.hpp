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
  
  Vector3(const float arData[3])
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
  
  Matrix(const Vector3& t_)
  : t(t_)
  {
    memcpy(arRot, karIdentityData, sizeof(arRot));
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
    if(resetTrans)
    {
      t = Vector3::s_zero;
    }
    
    if(setZeros)
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
    if(resetTrans)
    {
      t = Vector3::s_zero;
    }

    if(setZeros)
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
    if(resetTrans)
    {
      t = Vector3::s_zero;
    }

    if(setZeros)
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
    if(dz > .0f)
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
  
  void  SetAxisAngle(float x, float y, float z, float theta)
  {
    theta *= .5f;
    w = cosf(theta);

    float sinTheta(sinf(theta));
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

    theta *= 2;
  }
  
  void  GetAxisAngle(Vector3& v, float theta)
  {
    GetAxisAngle(v.x, v.y, v.z, theta);
  }
  
  void  RotateVec(Vector3& v)
  {
    float px((w * v.x) - (j * v.z) + (k * v.y));
    float py((w * v.y) - (k * v.x) + (i * v.z));
    float pz((w * v.z) - (i * v.y) + (j * v.x));

    float wpx(w * px);
    float wpy(w * py);
    float wpz(w * pz);
    
    float vxpx((j * pz) - (k * py));
    float vxpy((k * px) - (i * pz));
    float vxpz((i * py) - (j * px));

    float vxi2(v.x * i * i);
    float vyj2(v.y * j * j);
    float vzk2(v.z * k * k);
    
    v.x = vxi2 + wpx - vxpx;
    v.y = vyj2 + wpy - vxpy;
    v.z = vzk2 + wpz - vxpz;
  }
  
  // operator overloads
  Quaternion& operator*=(const Quaternion& rhs)
  {
    float w0(w);
    float x0(i);
    float y0(j);
    float z0(k);

    float w1(rhs.w);
    float x1(rhs.i);
    float y1(rhs.j);
    float z1(rhs.k);

    w = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1;
    i = w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1;
    j = w0 * y1 - x0 * z1 + y0 * w1 + z0 * x1;
    k = w0 * z1 + x0 * y1 - y0 * x1 + z0 * w1;
    return *this;
  }
  
  Quaternion  operator*(const Quaternion& rhs) const
  {
    Quaternion temp(*this);
    return temp *= rhs;
  }
  
  operator Matrix() const
  {
    float xy(2.0f * i * j);
    float wz(2.0f * w * k);
    float xz(2.0f * i * k);
    float wy(2.0f * w * j);
    float yz(2.0f * j * k);
    float wx(2.0f * w * i);

    float wSqr(w * w);
    float xSqr(i * i);
    float ySqr(j * j);
    float zSqr(k * k);

    float arMatrix[kNumMatrixInds] =
    {
      wSqr + xSqr - ySqr - zSqr, xy - wz, xz + wy,
      xy + wz, wSqr - xSqr + ySqr - zSqr, yz - wx,
      xz - wy, yz + wx, wSqr - xSqr - ySqr + zSqr
    };
    return Matrix(arMatrix);
  }
};

} // XRz

#endif // XR_VECTYPES_HPP