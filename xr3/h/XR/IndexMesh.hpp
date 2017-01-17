//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_INDEXMESH_HPP
#define XR_INDEXMESH_HPP

#include "BasicMesh.hpp"
#include <vector>

namespace XR {

//==============================================================================
class IndexMesh:  public BasicMesh
{
public:
  // types
  typedef std::vector<uint16_t> IndexArray;
  
  // structors
  IndexMesh();
  virtual ~IndexMesh();
  
  // general
  IndexArray&       GetIndices();
  const IndexArray& GetIndices() const;
  
  void              SetIndexPattern(const uint16_t* pInds, int numInds, int repeat);
  void              SetIndexPattern(const uint16_t* pInds, int numInds,
                      uint16_t shift, int repeat);
  
  virtual void      Render();
  
protected:
  // data
  IndexArray  m_indices;
};

//==============================================================================
// implementation
//==============================================================================
inline
IndexMesh::IndexArray&  IndexMesh::GetIndices()
{
  return m_indices;
}

//==============================================================================
inline
const IndexMesh::IndexArray&  IndexMesh::GetIndices() const
{
  return m_indices;
}

} // XR

#endif // XR_INDEXMESH_HPP
