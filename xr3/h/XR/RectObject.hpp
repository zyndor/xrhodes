//
// Nuclear Heart Games
// XRhodes
//
// RectObject.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    28/07/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_RECTOBJECT_HPP
#define XR_RECTOBJECT_HPP

#include "AABB.hpp"
#include "vectypes.hpp"

namespace XR
{

//==============================================================================
///@brief A rectangular object with position and half-size information.
class RectObject 
{
public:
  // structors
  RectObject(const Vector2& pos = Vector2::s_zero);
  RectObject(float hw, float hh, const Vector2& pos = Vector2::s_zero);
  ~RectObject();

  // general use
  const Vector2& GetPosition() const;
  Vector2&       GetPosition();
  
  float  GetLeft() const;
  float  GetRight() const;
  float  GetTop() const;
  float  GetBottom() const;

  float  GetHalfWidth() const;
  float  GetHalfHeight() const;
  float& GetHalfWidthRef();
  float& GetHalfHeightRef();

  void  SetHalfWidth(float hw);
  void  SetHalfHeight(float hh);
  
  float CalculateRadius() const;
  
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
///@return  The position of the RectObject.
inline
Vector2& RectObject::GetPosition()
{
  return m_position;
}

//==============================================================================
///@return  The position of the RectObject.
inline
const Vector2& RectObject::GetPosition() const
{
  return m_position;
}

//==============================================================================
///@return  The halfWidth of the RectObject.
inline
float RectObject::GetHalfWidth() const
{
  return m_halfWidth;
}

//==============================================================================
///@return  The halfHeight of the RectObject.
inline
float RectObject::GetHalfHeight() const
{
  return m_halfHeight;
}

//==============================================================================
///@return  A reference to the halfWidth of the RectObject.
inline
float& RectObject::GetHalfWidthRef()
{
  return m_halfWidth;
}

//==============================================================================
///@return  A reference to the halfHeight of the RectObject.
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
  return m_position.y - m_halfHeight;
}

//==============================================================================
inline
float  RectObject::GetBottom() const
{
  return m_position.y + m_halfHeight;
}

//==============================================================================
///@brief Calculates the radius of the RectObject. Useful when the object can
/// rotate.
inline
float RectObject::CalculateRadius() const
{
  return sqrtf((m_halfWidth * m_halfWidth) + (m_halfHeight * m_halfHeight));
}

//==============================================================================
///@brief Convenience method to import the object into the AABB @a box.
inline
void  RectObject::Export(AABB& box) const
{
  box.Import(m_position.x, m_position.y, m_halfWidth, m_halfHeight);
}

} // XR

#endif  //XR_RECTOBJECT_HPP