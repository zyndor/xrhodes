//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/meshutil.hpp"

namespace XR
{
namespace meshutil
{

//==============================================================================
void CalculateTrianglesNormals(uint32_t numVertices, Vector3 const * positions, uint32_t stride, Vector3 * normals)
{
  XR_ASSERT(CalculateTrianglesNormals, numVertices % 3 == 0);
  auto end = XR_ADVANCE_PTR(positions, numVertices * stride);
  while (positions != end)
  {
    const auto& a = *positions;
    positions = XR_ADVANCE_PTR(positions, stride);

    const auto& b = *positions - a;
    positions = XR_ADVANCE_PTR(positions, stride);

    const auto& c = *positions - a;
    positions = XR_ADVANCE_PTR(positions, stride);

    const Vector3& normal = b.Cross(c).Normalise();
    *normals = normal;
    normals = XR_ADVANCE_PTR(normals, stride);
    *normals = normal;
    normals = XR_ADVANCE_PTR(normals, stride);
    *normals = normal;
    normals = XR_ADVANCE_PTR(normals, stride);
  }
}

}
}
