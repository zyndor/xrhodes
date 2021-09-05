//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/io/streamutils.hpp"
#include "xr/memory/memdebug.hpp"
#include "xr/utils.hpp"
#include "xr/debug.hpp"
#include <algorithm>
#include <cstring>

using namespace xr;

namespace
{
  struct A
  {
    struct B
    {
      uint64_t u64;
      double d;
    };

    char chars[256];
    int32_t ints[3];
    float floats[6];
    B bs[2];

    A()
    {
      // The value of padding bytes are undefined. Since we're
      // taking the memcmp(Whole thing) shortcut, we'll need to
      // define them here.
      std::memset(this, 0x69, sizeof(A));
    }

    void Init()
    {
      for (uint32_t i = 0; i < XR_ARRAY_SIZE(chars); ++i)
      {
        chars[i] = char(i);
      }

      ints[0] = std::numeric_limits<int>::min();
      ints[1] = std::numeric_limits<int>::max();
      ints[2] = std::numeric_limits<int>::max();

      floats[0] = 1.0f;
      floats[1] = std::numeric_limits<float>::quiet_NaN();
      floats[2] = std::numeric_limits<float>::infinity();
      floats[3] = std::numeric_limits<float>::max();
      floats[4] = std::numeric_limits<float>::min();
      floats[5] = std::numeric_limits<float>::epsilon();

      bs[0].u64 = 0xffeeffeeffeeffee;
      bs[0].d = std::numeric_limits<double>::infinity();
      bs[1].u64 = 0x88aa88aa88aa88aa;
      bs[1].d = std::numeric_limits<double>::max();
    }

    size_t WritePieceWise(std::ostream& os)
    {
      size_t bytesWritten = 0;
      WriteRangeBinaryStream(chars, chars + XR_ARRAY_SIZE(chars), os, &bytesWritten);

      for (size_t i = 0; i < XR_ARRAY_SIZE(ints); ++i)
      {
        WriteBinaryStream(ints[i], os, &bytesWritten);
      }

      for (size_t i = 0; i < XR_ARRAY_SIZE(floats); ++i)
      {
        WriteBinaryStream(floats[i], os, &bytesWritten);
      }

      for (size_t i = 0; i < XR_ARRAY_SIZE(bs); ++i)
      {
        WriteBinaryStream(bs[i], os, &bytesWritten);
      }
      return bytesWritten;
    }

    void WriteWhole(std::ostream& os)
    {
      WriteBinaryStream(*this, os);
    }

    template <typename T>
    struct OutputIterator
    {
      OutputIterator(T& value):
        m_p(&value)
      {}

      T& operator*()
      {
        T& result = *m_p;
        ++m_p;
        return result;
      }

      T* m_p;
    };

    size_t ReadPiecewise(std::istream& is)
    {
      size_t bytesRead = 0;
      ReadRangeBinaryStream(is, OutputIterator<char>(chars[0]), &bytesRead);

      for (size_t i = 0; i < XR_ARRAY_SIZE(ints); ++i)
      {
        ReadBinaryStream(is, ints[i], &bytesRead);
      }

      for (size_t i = 0; i < XR_ARRAY_SIZE(floats); ++i)
      {
        ReadBinaryStream(is, floats[i], &bytesRead);
      }

      for (size_t i = 0; i < XR_ARRAY_SIZE(bs); ++i)
      {
        ReadBinaryStream(is, bs[i], &bytesRead);
      }

      return bytesRead;
    }

    void ReadWhole(std::istream& is)
    {
      ReadBinaryStream(is, *this);
    }
  };
}

XM_TEST(StreamUtils, ReadPiecewise)
{
  A a;
  a.Init();

  std::stringstream io;
  a.WritePieceWise(io);

  io.seekg(0, std::ios_base::beg);

  A a2;
  a2.ReadPiecewise(io);

  auto str1 = LogMemory(&a, sizeof(A));
  auto str2 = LogMemory(&a2, sizeof(A));
  auto diff = DiffMemory(&a, &a2, sizeof(A));
  XR_TRACE(StreamUtils, ("%s", str1.c_str()));
  XR_TRACE(StreamUtils, ("%s", str2.c_str()));
  XR_TRACE(StreamUtils, ("%s", diff.c_str()));
  XM_ASSERT_EQ(memcmp(&a, &a2, sizeof(A)), 0);
}

XM_TEST(StreamUtils, ReadWhole)
{
  A a;
  a.Init();

  std::stringstream io;
  a.WriteWhole(io);

  io.seekg(0, std::ios_base::beg);

  A a2;
  a2.ReadWhole(io);

  auto str1 = LogMemory(&a, sizeof(A));
  auto str2 = LogMemory(&a2, sizeof(A));
  auto diff = DiffMemory(&a, &a2, sizeof(A));
  XR_TRACE(StreamUtils, ("%s", str1.c_str()));
  XR_TRACE(StreamUtils, ("%s", str2.c_str()));
  XR_TRACE(StreamUtils, ("%s", diff.c_str()));
  XM_ASSERT_EQ(memcmp(&a, &a2, sizeof(A)), 0);
}

// NOTE: mixing piecewise and whole reads will not work due to struct alignment.
