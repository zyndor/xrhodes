//
// Nuclear Heart Studios
// XRhodes
//
// Indexer2d.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  15/06/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_INDEXER2D_HPP
#define XR_INDEXER2D_HPP

#include  "types.hpp"

namespace XR
{

//==============================================================================
///@brief Facilitates conversion between 2d grid coordinates and
/// 1d array indices.
class  Indexer2D
{
public:
  // structors
  Indexer2D();
  Indexer2D(int w, int h);
  ~Indexer2D();

  // general use
  ///@return  The width of the array.
  int  GetWidth() const;

  ///@return  The height of the array.
  int  GetHeight() const;

  ///@return  The total number of elements in the array.
  int  GetSize() const;

  ///@brief Sets the width of the array.
  void  SetWidth(int w);

  ///@brief Sets the height of the array.
  void  SetHeight(int h);

  ///@brief Sets the size of the array in one go.
  void  SetSize(int w, int h);

  ///@brief Converts @a x and @a y coordinates to an index into the array.
  ///@return  The index into the array that @a x and @a y correspond to.
  int   ToIndex(int x, int y) const;

  ///@brief Converts @a i to @a x and @a y grid coordinates.
  void  ToCoordinates(int i, int& x, int& y) const;

private:
  // internal
  ///@brief Calculates the size of the array.
  ///@return  The size of the array.
  int  _CalculateSize() const;

  // data
  int  m_width;
  int  m_height;
  int  m_size;
};

//==============================================================================
// implementation
//==============================================================================
inline
int  Indexer2D::GetWidth() const
{
  return m_width;
}

//==============================================================================
inline
int  Indexer2D::GetHeight() const
{
  return m_height;
}

//==============================================================================
inline
int  Indexer2D::GetSize() const
{
  return m_size;
}

}  // end namespace XR

#endif  //XR_INDEXER2D_HPP
