//
// Nuclear Heart Games
// XRhodes
//
// IndexMesh.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    05/02/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_INDEXMESH_HPP
#define XR_INDEXMESH_HPP

#include "BasicMesh.hpp"

namespace XR {

//==============================================================================
class IndexMesh:  public BasicMesh
{
public:
  // types
  typedef std::vector<uint16> IndexArray;
  
  // structors
  IndexMesh();
  virtual ~IndexMesh();
  
  // general
  IndexArray&       GetIndices();
  const IndexArray& GetIndices() const;
  
  void              SetIndexPattern(const uint16* pInds, int numInds, int repeat);
  void              SetIndexPattern(const uint16* pInds, int numInds,
                      uint16 shift, int repeat);
  
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
