#ifndef XR_MESHUTILS_HPP
#define XR_MESHUTILS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector3.hpp"
#include "xr/debug.hpp"
#include "xr/types/typeutils.hpp"
#include <cstdint>
#include <type_traits>
#include <limits>

namespace xr
{
namespace meshutil
{

//==============================================================================
///@brief moves pointer @a p by @a stride bytes.
#define XR_ADVANCE_PTR(p, stride) \
  reinterpret_cast<decltype(p)>(reinterpret_cast<typename TypeSelect<std::is_const<typename std::remove_pointer<decltype(p)>::type>::value, const uint8_t*, uint8_t*>::Type>(p) + stride)

//==============================================================================
///@brief Calculates the origin of a mesh with the given vertex positions.
template <class PosType>
PosType CalculateCentre(uint32_t numVertices, PosType const* positions, uint32_t stride)
{
  XR_ASSERT(meshutil, positions != nullptr);

  PosType centre = PosType::Zero();
  if (numVertices > 0)
  {
    for (uint32_t i = 0; i < numVertices; ++i)
    {
      centre += *positions;

      positions = XR_ADVANCE_PTR(positions, stride);
    }
    centre *= 1.0f / numVertices;
  }
  return centre;
}

//==============================================================================
///@brief Calculates the origin of a mesh with the given vertices.
template <class VertexFormat>
inline
decltype(VertexFormat::pos) CalculateCentre(uint32_t numVertices, VertexFormat const* vertices)
{
  XR_ASSERT(meshutil, vertices != nullptr);

  return CalculateCentre(numVertices, &vertices->pos, VertexFormat::kSize);
}

//==============================================================================
///@brief Calculates the distance of the farthest vertex from the origin.
template <class PosType>
float CalculateRadius(uint32_t numVertices, PosType const* positions, uint32_t stride)
{
  XR_ASSERT(meshutil, positions != nullptr);

  float radius = .0f;
  if (numVertices > 0)
  {
    for (uint32_t i = 0; i < numVertices; ++i)
    {
      float d = positions->Dot();
      if (d > radius)
      {
        radius = d;
      }

      positions = XR_ADVANCE_PTR(positions, stride);
    }
    radius = sqrtf(radius);
  }
  return radius;
}

//==============================================================================
///@brief Calculates the distance of the farthest vertex from the origin.
template <class VertexFormat>
inline
float CalculateRadius(uint32_t numVertices, VertexFormat const* vertices)
{
  XR_ASSERT(meshutil, vertices != nullptr);

  return CalculateRadius(numVertices, &vertices->pos, VertexFormat::kSize);
}

//==============================================================================
///@brief Calculates the minimum and maximum x, y, z coordinates for a bounding
/// box that contains the given vertices.
template <class PosType>
void CalculateExtents(uint32_t numVertices, PosType const* positions, uint32_t stride,
  PosType& outMin, PosType& outMax)
{
  XR_ASSERT(meshutil, positions != nullptr);

  if (numVertices > 0)
  {
    for (int i = 0; i < PosType::kNumComponents; ++i)
    {
      outMin.data[i] = std::numeric_limits<float>::max();
      outMax.data[i] = -std::numeric_limits<float>::max();
    }

    for (uint32_t i = 0; i < numVertices; ++i)
    {
      for (int j = 0; j < PosType::kNumComponents; ++j)
      {
        if (positions->data[j] < outMin.data[j])
        {
          outMin.data[j] = positions->data[j];
        }

        if (positions->data[j] > outMax.data[j])
        {
          outMax.data[j] = positions->data[j];
        }
      }

      positions = XR_ADVANCE_PTR(positions, stride);
    }
  }
  else
  {
    outMin = outMax = PosType::Zero();
  }
}

//==============================================================================
///@brief Calculates the minimum and maximum x, y, z coordinates for a bounding
/// box that contains the given vertices.
template <class VertexFormat>
inline
void CalculateExtents(uint32_t numVertices, VertexFormat const* vertices,
  decltype(VertexFormat::pos)& outMin, decltype(VertexFormat::pos)& outMax)
{
  XR_ASSERT(meshutil, vertices != nullptr);

  CalculateExtents(numVertices, &vertices->pos, VertexFormat::kSize, outMin, outMax);
}

//==============================================================================
///@brief Calculates the normals for the given vertices, which it assumes to
/// be a list of triangles.
void CalculateTrianglesNormals(uint32_t numVertices, Vector3 const* positions,
  uint32_t stride, Vector3* normals);

//==============================================================================
///@brief Calculates the normals for the given vertices, which it assumes to
/// be a list of triangles.
template <class VertexFormat>
inline
void CalculateTrianglesNormals(uint32_t numVertices, VertexFormat* vertices)
{
  CalculateTrianglesNormals(numVertices, &vertices->pos, VertexFormat::kSize,
    &vertices->normal);
}

//==============================================================================
///@brief Writes a pattern of @a indices, @a numSets times, to @a outIndices,
/// applying an @a offset after each set, i.e.: { 0, 1, 2 }, offset: 5, sets: 2 ->
/// { 0, 2, 1, 5, 7, 6 }.
///@note @a outIndices must have space for @a indexCount x @a numSets elements.
inline
void SetIndexPattern(const uint16_t* indices, uint32_t indexCount, uint16_t offset,
  uint32_t numSets, uint16_t* outIndices)
{
  XR_ASSERT(meshutil, indices != nullptr);
  XR_ASSERT(meshutil, outIndices != nullptr);

  uint16_t vertexOffset = 0;
  for (decltype(numSets) i = 0; i < numSets; ++i)
  {
    for (decltype(indexCount) j = 0; j < indexCount; ++j)
    {
      *outIndices = indices[j] + vertexOffset;
      ++outIndices;
    }
    vertexOffset += offset;
  }
}

//==============================================================================
///@brief Writes a pattern of @a indices, @a numSets times, to @a outIndices,
/// applying an offset after each set, based on the highest index in the pattern.
/// i.e.: { 0, 1, 2 }, sets: 2 -> { 0, 2, 1, 3, 5, 4 }.
///@note @a outIndices must have space for @a indexCount x @a numSets elements.
inline
void SetIndexPattern(const uint16_t* indices, uint32_t indexCount, uint32_t numSets,
  uint16_t* outIndices)
{
  XR_ASSERT(meshutil, indices != nullptr);
  XR_ASSERT(meshutil, outIndices != nullptr);

  uint16_t  shift = 0;
  for (decltype(indexCount) i = 0; i < indexCount; ++i)
  {
    if (indices[i] > shift)
    {
      shift = indices[i];
    }
  }

  SetIndexPattern(indices, indexCount, shift + 1, numSets, outIndices);
}

} // meshutil
} // xr

#endif //XR_MESHUTILS_HPP
