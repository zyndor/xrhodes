#include "gtest/gtest.h"
#include "XR/streamutils.hpp"
#include "XR/memdebug.hpp"
#include "XR/utils.hpp"
#include "XR/debug.hpp"
#include <algorithm>

namespace XR
{
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
        for (int i = 0; i < XR_ARRAY_SIZE(chars); ++i)
        {
          chars[i] = i;
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

        for (int i = 0; i < XR_ARRAY_SIZE(ints); ++i)
        {
          WriteBinaryStream(ints[i], os, &bytesWritten);
        }
        for (int i = 0; i < XR_ARRAY_SIZE(floats); ++i)
        {
          WriteBinaryStream(floats[i], os, &bytesWritten);
        }
        for (int i = 0; i < XR_ARRAY_SIZE(bs); ++i)
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
        OutputIterator(T& value)
          : m_p(&value)
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

        for (int i = 0; i < XR_ARRAY_SIZE(ints); ++i)
        {
          ReadBinaryStream(is, ints[i], &bytesRead);
        }
        for (int i = 0; i < XR_ARRAY_SIZE(floats); ++i)
        {
          ReadBinaryStream(is, floats[i], &bytesRead);
        }
        for (int i = 0; i < XR_ARRAY_SIZE(bs); ++i)
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

  TEST(StreamUtils, ReadPiecewise)
  {
    A a;
    a.Init();

    std::stringstream io;
    a.WritePieceWise(io);

    io.seekg(0, std::ios_base::beg);

    A a2;
    a2.ReadPiecewise(io);

    XR_TRACE(StreamUtilsTests, (LogMemory(&a, sizeof(A)).c_str()));
    XR_TRACE(StreamUtilsTests, (LogMemory(&a2, sizeof(A)).c_str()));
    XR_TRACE(StreamUtilsTests, (DiffMemory(&a, &a2, sizeof(A)).c_str()));
    ASSERT_EQ(memcmp(&a, &a2, sizeof(A)), 0);
  }

  TEST(StreamUtils, ReadWhole)
  {
    A a;
    a.Init();

    std::stringstream io;
    a.WriteWhole(io);

    io.seekg(0, std::ios_base::beg);

    A a2;
    a2.ReadWhole(io);

    XR_TRACE(StreamUtilsTests, (LogMemory(&a, sizeof(A)).c_str()));
    XR_TRACE(StreamUtilsTests, (LogMemory(&a2, sizeof(A)).c_str()));
    XR_TRACE(StreamUtilsTests, (DiffMemory(&a, &a2, sizeof(A)).c_str()));
    ASSERT_EQ(memcmp(&a, &a2, sizeof(A)), 0);
  }

  // NOTE: mixing piecewise and whole reads will not work due to struct alignment.
}
