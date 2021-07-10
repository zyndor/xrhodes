#ifndef XR_INDEXER2D_HPP
#define XR_INDEXER2D_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Facilitates conversion between 2d grid coordinates and
/// 1d array indices.
class  Indexer2d
{
public:
  // structors
  Indexer2d();
  Indexer2d(int32_t w, int32_t h);
  ~Indexer2d();

  // general use
  ///@return  The width of the array.
  int32_t  GetWidth() const;

  ///@return  The height of the array.
  int32_t  GetHeight() const;

  ///@return  The total number of elements in the array.
  int32_t  GetSize() const;

  ///@brief Sets the width of the array.
  void  SetWidth(int32_t w);

  ///@brief Sets the height of the array.
  void  SetHeight(int32_t h);

  ///@brief Sets the size of the array in one go.
  void  SetSize(int32_t w, int32_t h);

  ///@brief Converts @a x and @a y coordinates to an index into the array.
  ///@return  The index into the array that @a x and @a y correspond to.
  int32_t   ToIndex(int32_t x, int32_t y) const;

  ///@brief Converts @a i to @a x and @a y grid coordinates.
  void  ToCoordinates(int32_t i, int32_t& x, int32_t& y) const;

private:
  // internal
  ///@brief Calculates the size of the array.
  ///@return  The size of the array.
  int32_t  CalculateSize() const;

  // data
  int32_t  m_width;
  int32_t  m_height;
  int32_t  m_size;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32_t  Indexer2d::GetWidth() const
{
  return m_width;
}

//==============================================================================
inline
int32_t  Indexer2d::GetHeight() const
{
  return m_height;
}

//==============================================================================
inline
int32_t  Indexer2d::GetSize() const
{
  return m_size;
}

}  // end namespace XR

#endif  //XR_INDEXER2D_HPP
