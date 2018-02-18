#ifndef XR_SDFBUILDER_HPP
#define XR_SDFBUILDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/debug.hpp"
#include "XR/maths.hpp"
#include <vector>
#include <cstdint>

namespace XR
{

//==============================================================================
// Generates Signed Distance Field from 8-bit monochrome bitmaps.
class SdfBuilder
{
public:
  struct Point
  {
    int x;
    int y;
  };

  SdfBuilder(uint32_t bitmapSize, uint32_t fieldSize)
  : m_rowSize(bitmapSize + 2),
    m_points(m_rowSize * m_rowSize),
    m_distances(m_points.size(), .0f),
    m_fieldSize(fieldSize)
  {}

  void Generate(uint8_t const* srcPixels, uint32_t srcRowSize, uint32_t w, uint32_t h)
  {
    const int rowSize = m_rowSize;
    XR_ASSERT(SdfBuilder, w <= rowSize - 2);
    XR_ASSERT(SdfBuilder, h <= rowSize - 2);
    // Clear buffers.
    std::fill(m_points.begin(), m_points.end(), Point{ -1, -1 });
    std::fill(m_distances.begin(), m_distances.end(), std::numeric_limits<float>::max());

    const size_t idxBegin = rowSize + 1; // index of first useful pixel.
    Point* p = m_points.data() + idxBegin;
    float* d = m_distances.data() + idxBegin;

    // Find border points - points whose value is different from that of its
    // neighbours. Set their distance to 0.
    auto iSrc = srcPixels;
    const auto rowDiff = rowSize - w;
    const auto srcRowDiff = srcRowSize - w;
    for (int y = 0; y < h; ++y)
    {
      auto p0 = p;
      auto p1 = p + w;
      while (p != p1)
      {
        auto value = *iSrc;
        if (*(iSrc - 1) != value || *(iSrc + 1) != value ||
          *(iSrc + srcRowSize) != value || *(iSrc - srcRowSize) != value)
        {
          *d = .0f;
          *p = { int(p - p0), y };
        }
        ++d;
        ++p;
        ++iSrc;
      }
      p += rowDiff;
      d += rowDiff;
      iSrc += srcRowDiff;
    }

    // Calculate distances from closest border point. Once the neighbour of
    // a border point was found, this closest point will propagate, creating
    // the distance field. For this to happen, however, we will need two
    // passes, the first one of which will check left and above.
    const float d1 = 1.0f;
    const float d2 = sqrtf(2.0f);
    p = m_points.data() + idxBegin;
    d = m_distances.data() + idxBegin;
    for (int y = 0; y < h; ++y)
    {
      for (int x = 0; x < w; ++x)
      {
        int offset = -(rowSize + 1); // top left
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = -rowSize; // above
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = -(rowSize - 1); // top right
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = -1;  // left
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        ++p;
        ++d;
      }
      p += rowDiff;
      d += rowDiff;
    }

    // Second pass, check bottom and right, starting from bottom right corner.
    auto wLess1 = w - 1;
    auto hLess1 = h - 1;
    auto idxEnd = idxBegin + wLess1 + hLess1 * rowSize;
    p = m_points.data() + idxEnd;
    d = m_distances.data() + idxEnd;
    for (int y = hLess1; y >= 0; --y)
    {
      for (int x = wLess1; x >= 0; --x)
      {
        int offset = 1; // right
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = rowSize - 1;  // bottom left
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = rowSize;  // below
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        offset = rowSize + 1;  // bottom right
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = sqrt(dx * dx + dy * dy);
          *p = b;
        }

        --p;
        --d;
      }
      p -= rowDiff;
      d -= rowDiff;
    }

    // Scale distance field values into the -127, 128 range, clamping them first
    // to the -fieldSize, fieldSize range. This is to eliminate minification
    // artifacts.
    const float scale = m_fieldSize;
    d = m_distances.data() + idxBegin;
    iSrc = srcPixels;
    for (int y = 0; y < h; ++y)
    {
      for (int x = 0; x < w; ++x)
      {
        if (*iSrc == 0)
        {
          *d = -*d;
        }

        float dist = Clamp(*d, -scale, scale) * (128.0f / scale);
        *d = Clamp(dist, -127.0f, 128.0f);
        ++d;
        ++iSrc;
      }
      iSrc += srcRowDiff;
      d += rowDiff;
    }
  }

  void ConvertToBitmap(uint32_t w, uint32_t h, uint8_t* buffer)
  {
    XR_ASSERT(SdfBuilder, w <= m_rowSize - 2);
    XR_ASSERT(SdfBuilder, h <= m_rowSize - 2);
    float* d = m_distances.data() + m_rowSize + 1;
    for (int y = 0; y < h; ++y)
    {
      for (int x = 0; x < w; ++x)
      {
        float value = d[x + m_rowSize * y] + 127.0f;
        buffer[x + w * y] = uint8_t(value);
      }
    }
  }

private:
  size_t m_rowSize;
  std::vector<Point> m_points;
  std::vector<float> m_distances;
  float m_fieldSize;
};

}

#endif
