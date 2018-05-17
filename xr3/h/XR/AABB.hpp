#ifndef XR_AABB_HPP
#define XR_AABB_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "maths.hpp"

namespace XR
{

//==============================================================================
///@brief Axis Aligned Bounding Box class with support for Import()ing
/// from / Export()ing to halfsize information and for HitTest()s.
///@note Assumes the positive Y axis pointing downwards. 
struct  AABB
{
  // types
  enum  Tags
  {
    ///@brief Must be specified.
    LEFT,
    ///@brief Must be specified.
    TOP,
    ///@brief Must be specified.
    RIGHT,
    ///@brief Must be specified.
    BOTTOM,
    kNumTags
  };

  ///@brief Information about a HitTest() between two AABBs.
  ///@note  When removing an object from a collision, you will need to add the
  /// overlap to the relevant coordinate of its position, or subtract from it
  /// if the object is on the far side of the other object.
  struct  HitTestInfo
  {
    ///@brief Whether a hit has occurred.
    bool  hit;
    ///@brief The amount of overlap along the X axis.
    float  xOverlap;
    ///@brief The amount of overlap along the Y axis.
    float  yOverlap;

    ///@return  Whether the collision happened along the x axis so that you will
    /// work with xOverlap.
    ///@note  Only makes sense if there was a collision, i.e. hit is true.
    bool  IsXCollision() const;

    ///@return  Whether the collision happened along the y axis so that you will
    /// work with yOverlap.
    ///@note  Only makes sense if there was a collision, i.e. hit is true.
    bool  IsYCollision() const;
  };

  // static
  static const char* const kTags[kNumTags];

  // data
  ///@brief The left side of the AABB.
  float left;
  ///@brief The top side of the AABB.
  float top;
  ///@brief The right side of the AABB.
  float right;
  ///@brief The bottom side of the AABB.
  float bottom;

  // general use
  ///@brief Converts position and halfsize information into world coordinates
  /// of the four sides of the AABB.
  void  Import(float x, float y, float hs);

  ///@brief Converts position and halfsize information into world coordinates
  /// of the four sides of the AABB.
  void  Import(float x, float y, float hw, float hh);

  ///@brief Converts world coordinates of the four sides of the AABB
  /// to position and halfsize information.
  void  Export(float& x, float& y, float& hw, float& hh) const;

  ///@return The object's presence on the X axis. You can use the this to compare
  /// the relative position of AABBs.
  float  CalculateXness() const;

  ///@return The object's presence on the Y axis. You can use the this to compare
  /// the relative position of AABBs.
  float  CalculateYness() const;

  ///@brief Tests for overlap between this AABB and a 2D point.
  ///@return  If there was an overlap.
  bool  HitTest(float x, float y) const;

  ///@brief Tests for overlap between this AABB and a 2D point.
  /// The overlap information is stored in the HitTestInfo object.
  ///@return  If there was an overlap.
  bool  HitTest(float x, float y, HitTestInfo& inf) const;

  ///@brief Tests for overlap between this AABB and an @a other one.
  ///@return  If there was an overlap.
  bool  HitTest(const AABB& other) const;

  ///@brief Tests for overlap between this AABB and an @a other one.
  /// The overlap information is stored in @a inf.
  ///@return  If there was an overlap.
  bool  HitTest(const AABB& other, HitTestInfo& inf) const;

  ///@brief Tests for intersection between this AABB and a 2D line segment
  /// defined by start coordinates @a x0, @a y0 and extents @a dx and @a dy.
  ///@return  If there was an overlap.
  bool  HitTest(float x0, float y0, float dx, float dy) const;

  ///@brief Tests for intersection between this AABB and a 2D line segment
  /// defined by start coordinates @a x0, @a y0 and extents @a dx and @a dy.
  /// The parameter of the intersection, if any, is stored in @a t. @a x0 +
  /// @a t * @a dx, @a y0 + @a y * @a dy yields the intersection point in such
  /// case.
  ///@return  If there was an overlap.
  bool  HitTest(float x0, float y0, float dx, float dy, float& t) const;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  AABB::HitTestInfo::IsXCollision() const
{
  return xOverlap > yOverlap;
}

//==============================================================================
inline
bool  AABB::HitTestInfo::IsYCollision() const
{
  return yOverlap > xOverlap;
}

//==============================================================================
inline
void  AABB::Import(float x, float y, float hs)
{
  Import(x, y, hs, hs);
}

//==============================================================================
inline
float AABB::CalculateXness() const
{
  return left + right;
}

//==============================================================================
inline
float AABB::CalculateYness() const
{
  return bottom + top;
}

//==============================================================================
inline
bool  AABB::HitTest(float x, float y) const
{
  return std::max(x - right, left - x) < .0f &&
    std::max(y - bottom, top - y) < .0f;
}

//==============================================================================
inline
bool  AABB::HitTest(float x, float y, HitTestInfo& inf) const
{
  inf.xOverlap = std::max(x - right, left - x);
  inf.yOverlap = std::max(y - bottom, top - y);
  inf.hit = inf.xOverlap < .0f && inf.yOverlap < .0f;
  return inf.hit;
}

//==============================================================================
inline
bool  AABB::HitTest(const AABB& other) const
{
  return std::max(other.left - right, left - other.right) < .0f &&
    std::max(other.bottom - top, bottom - other.top) < .0f;
}

//==============================================================================
inline
bool  AABB::HitTest(const AABB& other, HitTestInfo& inf) const
{
  inf.xOverlap = std::max(other.left - right, left - other.right);
  inf.yOverlap = std::max(other.bottom - top, bottom - other.top);
  inf.hit = inf.xOverlap < .0f && inf.yOverlap < .0f;
  return inf.hit;
}

}  // XR

#endif  //XR_AABB_HPP
