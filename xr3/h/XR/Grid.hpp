//
// Nuclear Heart Studios
// XRhodes
//
// Grid.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  20/06/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_GRID_HPP
#define XR_GRID_HPP

#include "maths.hpp"

namespace XR
{

//==============================================================================
///@brief Facilitates conversion from world coordinates
/// to grid coordinates.
class  Grid
{
public:
  // static
  static const float  kMinSize;

  // structors
  Grid();
  explicit Grid(float size);
  ~Grid();

  // general use
  ///@return  The size of the grid.
  float GetSize() const;

  ///@return  The halfsize of the grid.
  float GetHalfSize() const;

  ///@return  The reciprocal of the size of the grid.
  float GetSizeRec() const;

  ///@brief Sets the size of the grid.
  void  SetSize(float size);

  ///@brief Converts a world coordinate to grid coordinate.
  ///@return  The grid coordinate the @a world coordinate is in.
  int   ToGrid(float world) const;

private:
  // data
  float m_size;
  float m_halfSize;
  float m_sizeRec;
};

//==============================================================================
// implementation
//==============================================================================
inline
float  Grid::GetSize() const
{
  return m_size;
}

//==============================================================================
inline
float  Grid::GetHalfSize() const
{
  return m_halfSize;
}

//==============================================================================
inline
float  Grid::GetSizeRec() const
{
  return m_sizeRec;
}

}  // end namespace XR

#endif  //XR_GRID_HPP
