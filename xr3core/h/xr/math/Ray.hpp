#ifndef XR_RAY_HPP
#define XR_RAY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector3.hpp"

namespace xr
{

//==============================================================================
///@brief 3D Ray.
class  Ray
{
public:
  // data
  Vector3 position;
  Vector3 direction;
  float   length;

  // structors
  Ray();
  Ray(const Vector3& pos, const Vector3& dir, float l);

  // general
  ///@brief Sets the direction of the Ray, normalised (which is what most of XR
  /// will expect it to be).
  void  SetDirection(const Vector3& dir);

  ///@brief Sets the direction of the Ray. Client code has to make sure that
  /// @a dir is normalised.
  void  SetDirectionNormalised(const Vector3& dir);

  ///@brief Intersects the sphere defined by @a pos and @a radiusSqr, with the
  /// ray.
  ///@return  Whether the ray hits the sphere.
  bool  IntersectSphere(const Vector3& pos, float radiusSqr) const;

  ///@brief Intersects the sphere defined by @a pos and @a radiusSqr, with the
  /// ray. Only if it hits, the results are stored in @a t0 and @a t1.
  /// Ray.position + ray.direction * {t0 ; t1} will then give you the point of
  /// intersection.
  ///@return  Whether the ray hits the sphere.
  bool  IntersectSphere(const Vector3& pos, float radiusSqr, float& t0,
          float& t1) const;

  ///@brief Intersects the plane defined by @a pos (distance from origin) and
  /// @a normal, with the ray.
  ///@return  Whether the ray hits the plane. False if they're parallel.
  bool  IntersectPlane(const Vector3& pos, const Vector3& normal) const;

  ///@brief Intersects the plane defined by @a pos (distance from origin) and
  /// @a normal, with the ray. Only if it hits, the result is stored in @a t.
  /// Ray.position + ray.direction * t will then give you the point of
  /// intersection.
  ///@return  Whether the ray hits the plane. False if they're parallel.
  bool  IntersectPlane(const Vector3& pos, const Vector3& normal, float& t) const;

  ///@brief Convenience function to calculate the point along the ray using the
  /// parameter @a t.
  Vector3 GetPoint(float t) const;

  ///@brief Convenience function to calculate the end point of the ray.
  Vector3 GetEnd() const;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  Ray::IntersectSphere(const Vector3& pos, float radiusSqr) const
{
  static float t0, t1;
  return IntersectSphere(pos, radiusSqr, t0, t1);
}

//==============================================================================
inline
bool  Ray::IntersectPlane(const Vector3& pos, const Vector3& normal) const
{
  static float t;
  return IntersectPlane(pos, normal, t);
}

//==============================================================================
inline
Vector3 Ray::GetPoint(float t) const
{
  return position + direction * t;
}

//==============================================================================
inline
Vector3 Ray::GetEnd() const
{
  return GetPoint(length);
}

} // XR

#endif // XR_RAY_HPP
