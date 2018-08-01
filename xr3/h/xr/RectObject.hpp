#ifndef XR_RECTOBJECT_HPP
#define XR_RECTOBJECT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Vector2.hpp"

namespace xr
{

struct AABB;

//==============================================================================
///@brief An axis aligned, rectangular object with position and half-size
/// information.
class RectObject
{
public:
  // structors
  RectObject(const Vector2& pos = Vector2::Zero());
  RectObject(float hw, float hh, const Vector2& pos = Vector2::Zero());
  ~RectObject();

  // general use
  ///@return  The position of the RectObject.
  const Vector2& GetPosition() const;

  ///@return  The position of the RectObject.
  Vector2&       GetPosition();

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
  Vector2 m_position;
  float   m_halfWidth;
  float   m_halfHeight;
};

//==============================================================================
// implementation
//==============================================================================
inline
Vector2& RectObject::GetPosition()
{
  return m_position;
}

//==============================================================================
inline
const Vector2& RectObject::GetPosition() const
{
  return m_position;
}

//==============================================================================
inline
float RectObject::GetHalfWidth() const
{
  return m_halfWidth;
}

//==============================================================================
inline
float RectObject::GetHalfHeight() const
{
  return m_halfHeight;
}

//==============================================================================
inline
float& RectObject::GetHalfWidthRef()
{
  return m_halfWidth;
}

//==============================================================================
inline
float& RectObject::GetHalfHeightRef()
{
  return m_halfHeight;
}

//==============================================================================
inline
float  RectObject::GetLeft() const
{
  return m_position.x - m_halfWidth;
}

//==============================================================================
inline
float  RectObject::GetRight() const
{
  return m_position.x + m_halfWidth;
}

//==============================================================================
inline
float  RectObject::GetTop() const
{
  return m_position.y + m_halfHeight;
}

//==============================================================================
inline
float  RectObject::GetBottom() const
{
  return m_position.y - m_halfHeight;
}

//==============================================================================
inline
float RectObject::CalculateRadius() const
{
  return sqrtf((m_halfWidth * m_halfWidth) + (m_halfHeight * m_halfHeight));
}

} // XR

#endif  //XR_RECTOBJECT_HPP
