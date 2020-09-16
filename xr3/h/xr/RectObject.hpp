#ifndef XR_RECTOBJECT_HPP
#define XR_RECTOBJECT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector2.hpp"
#include "xr/AABB.hpp"

namespace xr
{

class AABB;

//==============================================================================
///@brief An axis aligned, rectangular object with position and half-size
/// information.
class RectObject
{
public:
  // structors
  RectObject(Vector2 const& pos = Vector2::Zero());
  RectObject(float hw, float hh, Vector2 const& pos = Vector2::Zero());
  ~RectObject();

  // general use
  ///@return  The position of the RectObject.
  Vector2 const& GetPosition() const;

  ///@return  The position of the RectObject.
  Vector2&  GetPosition();

  ///@brief Coordinates of the left side.
  float  GetLeft() const;

  ///@brief Coordinates of the right side.
  float  GetRight() const;

  ///@brief Coordinates of the top side.
  float  GetTop() const;

  ///@brief Coordinates of the bottom side.
  float  GetBottom() const;

  ///@return  The halfWidth of the RectObject.
  float  GetHalfWidth() const;

  ///@return  The halfHeight of the RectObject.
  float  GetHalfHeight() const;

  ///@return  A reference to the half width of the RectObject.
  float& GetHalfWidthRef();

  ///@return  A reference to the half height of the RectObject.
  float& GetHalfHeightRef();

  ///@brief Sets the half width of the object.
  void  SetHalfWidth(float hw);

  ///@brief Sets the half height of the object.
  void  SetHalfHeight(float hh);

  ///@brief Calculates the radius of the RectObject. Useful when the object can
  /// rotate.
  float CalculateRadius() const;

  ///@brief Convenience method to import the object into the AABB @a box.
  void  Export(AABB& box) const;

protected:
  // data
  Vector2 mPosition;
  float   mHalfWidth;
  float   mHalfHeight;
};

//==============================================================================
// implementation
//==============================================================================
inline
Vector2& RectObject::GetPosition()
{
  return mPosition;
}

//==============================================================================
inline
Vector2 const& RectObject::GetPosition() const
{
  return mPosition;
}

//==============================================================================
inline
float RectObject::GetHalfWidth() const
{
  return mHalfWidth;
}

//==============================================================================
inline
float RectObject::GetHalfHeight() const
{
  return mHalfHeight;
}

//==============================================================================
inline
float& RectObject::GetHalfWidthRef()
{
  return mHalfWidth;
}

//==============================================================================
inline
float& RectObject::GetHalfHeightRef()
{
  return mHalfHeight;
}

//==============================================================================
inline
float  RectObject::GetLeft() const
{
  return mPosition.x - mHalfWidth;
}

//==============================================================================
inline
float  RectObject::GetRight() const
{
  return mPosition.x + mHalfWidth;
}

//==============================================================================
inline
float  RectObject::GetTop() const
{
  return mPosition.y + mHalfHeight;
}

//==============================================================================
inline
float  RectObject::GetBottom() const
{
  return mPosition.y - mHalfHeight;
}

//==============================================================================
inline
float RectObject::CalculateRadius() const
{
  return sqrtf((mHalfWidth * mHalfWidth) + (mHalfHeight * mHalfHeight));
}

} // xr

#endif  //XR_RECTOBJECT_HPP
