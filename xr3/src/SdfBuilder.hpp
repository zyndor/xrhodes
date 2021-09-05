#ifndef XR_SDFBUILDER_HPP
#define XR_SDFBUILDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include "xr/math/mathutils.hpp"
#include "xr/Px.hpp"
#include <vector>
#include <cstdint>

namespace xr
{

//==============================================================================
// Generates Signed Distance Field from 8-bit monochrome bitmaps.
class SdfBuilder
{
public:
  struct Point
  {
    Px x;
    Px y;
  };

  SdfBuilder(Px bitmapSize, Px fieldSize)
  : m_bitmapSize(bitmapSize + 2),  // pad by 1 pixel each side
    m_bitmapSizeUnpadded(bitmapSize),
    m_points(m_bitmapSize * m_bitmapSize),
    m_distances(m_points.size(), .0f),
    m_fieldSize(static_cast<float>(fieldSize))
  {}

  void Generate(uint8_t const* srcPixels, Px srcPitch, Px w, Px h)
  {
    const auto rowSizePadded = m_bitmapSize;
    XR_ASSERT(SdfBuilder, w <= m_bitmapSizeUnpadded);
    XR_ASSERT(SdfBuilder, h <= m_bitmapSizeUnpadded);

    // Clear buffers.
    std::fill(m_points.begin(), m_points.end(), Point{ Px(-1), Px(-1) });
    std::fill(m_distances.begin(), m_distances.end(), std::numeric_limits<float>::max());

    const size_t idxBegin = rowSizePadded + 1; // index of first useful pixel.
    Point* p = m_points.data() + idxBegin;
    float* d = m_distances.data() + idxBegin;

    // Find border points - points whose value is different from that of its
    // neighbours. Set their distance to 0.
    auto iSrc = srcPixels;
    const auto rowDiff = rowSizePadded - w;
    const auto srcRowDiff = srcPitch - w;
    for (decltype(h) y = 0; y < h; ++y)
    {
      auto p0 = p;
      auto p1 = p + w;
      while (p != p1)
      {
        auto value = *iSrc;
        if (*(iSrc - 1) != value || *(iSrc + 1) != value ||
          *(iSrc + srcPitch) != value || *(iSrc - srcPitch) != value)
        {
          *d = .0f;
          *p = { Px(p - p0), y };
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
    for (decltype(h) y = 0; y < h; ++y)
    {
      for (decltype(w) x = 0; x < w; ++x)
      {
        ptrdiff_t offset = -(static_cast<int32_t>(rowSizePadded) + 1); // top left
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = -static_cast<int32_t>(rowSizePadded); // above
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = -static_cast<int32_t>(rowSizePadded - 1); // top right
        if (*(d + offset) + d2 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = -1;  // left
        if (*(d + offset) + d1 < *d)
        {
          auto b = *(p + offset);
          int dx = x - b.x;
          int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        ++p;
        ++d;
      }
      p += rowDiff;
      d += rowDiff;
    }

    // Second pass, check bottom and right, starting from bottom right corner.
    const Px wLess1 = w - 1;
    const Px hLess1 = h - 1;
    auto idxEnd = idxBegin + wLess1 + hLess1 * rowSizePadded;
    p = m_points.data() + idxEnd;
    d = m_distances.data() + idxEnd;
    for (Px y = hLess1;; --y)
    {
      for (Px x = wLess1;; --x)
      {
        int offset = 1; // right
        if (*(d + offset) + d1 < *d)
        {
          const auto b = *(p + offset);
          const int dx = x - b.x;
          const int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = static_cast<int32_t>(rowSizePadded - 1);  // bottom left
        if (*(d + offset) + d2 < *d)
        {
          const auto b = *(p + offset);
          const int dx = x - b.x;
          const int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = static_cast<int32_t>(rowSizePadded);  // below
        if (*(d + offset) + d1 < *d)
        {
          const auto b = *(p + offset);
          const int dx = x - b.x;
          const int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        offset = static_cast<int32_t>(rowSizePadded + 1);  // bottom right
        if (*(d + offset) + d2 < *d)
        {
          const auto b = *(p + offset);
          const int dx = x - b.x;
          const int dy = y - b.y;
          *d = std::sqrt(float(dx * dx + dy * dy));
          *p = b;
        }

        --p;
        --d;

        if (x == 0)
        {
          break;
        }
      }

      p -= rowDiff;
      d -= rowDiff;

      if (y == 0)
      {
        break;
      }
    }

    // Scale distance field values into the -127, 128 range, clamping them first
    // to the -fieldSize, fieldSize range. This is to eliminate minification
    // artifacts.
    const float scale = m_fieldSize;
    d = m_distances.data() + idxBegin;
    iSrc = srcPixels;
    for (decltype(h) y = 0; y < h; ++y)
    {
      for (decltype(w) x = 0; x < w; ++x)
      {
        if (*iSrc == 0)
        {
          *d = -*d;
        }

        const float dist = Clamp(*d, -scale, scale) * (128.0f / scale);
        *d = Clamp(dist, -127.0f, 128.0f);
        ++d;
        ++iSrc;
      }
      iSrc += srcRowDiff;
      d += rowDiff;
    }
  }

  void ConvertToBitmap(Px w, Px h, uint8_t* buffer)
  {
    XR_ASSERT(SdfBuilder, w <= m_bitmapSizeUnpadded);
    XR_ASSERT(SdfBuilder, h <= m_bitmapSizeUnpadded);
    float* d = m_distances.data() + m_bitmapSize + 1;
    for (Px y = 0; y < h; ++y)
    {
      for (Px x = 0; x < w; ++x)
      {
        float value = d[x + m_bitmapSize * y] + 127.0f;
        buffer[x + w * y] = uint8_t(value);
      }
    }
  }

private:
  Px m_bitmapSize;
  Px m_bitmapSizeUnpadded;
  std::vector<Point> m_points;
  std::vector<float> m_distances;
  float m_fieldSize;
};

}

#endif
