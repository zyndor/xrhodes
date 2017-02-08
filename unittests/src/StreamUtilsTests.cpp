#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/streamutils.hpp>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(StreamUtilsTests)
  {
  public:
    struct A
    {
      struct B
      {
        uint64_t u64;
        double d;
      };

      char chars[256];
      int ints[3];
      float floats[6];
      B bs[2];

      void Init()
      {
        std::memset(this, 0x00, sizeof(A)); // value of bytes in padding is undefined, so let's define them for memcmp().

        for (int i = 0; i < 256; ++i)
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
        for (int i = 0; i < 256; ++i)
        {
          if (WriteBinaryStream(chars[i], os))
          {
            bytesWritten += sizeof(char);
          }
        }
        for (int i = 0; i < 3; ++i)
        {
          if (WriteBinaryStream(ints[i], os))
          {
            bytesWritten += sizeof(int);
          }
        }
        for (int i = 0; i < 6; ++i)
        {
          if (WriteBinaryStream(floats[i], os))
          {
            bytesWritten += sizeof(float);
          }
        }
        for (int i = 0; i < 2; ++i)
        {
          if (WriteBinaryStream(bs[i], os))
          {
            bytesWritten += sizeof(B);
          }
        }
        return bytesWritten;
      }

      void WriteWhole(std::ostream& os)
      {
        WriteBinaryStream(*this, os);
      }

      size_t ReadPiecewise(std::istream& is)
      {
        size_t bytesRead = 0;
        for (int i = 0; i < 256; ++i)
        {
          if (ReadBinaryStream(is, chars[i]))
          {
            bytesRead += sizeof(char);
          }
        }
        for (int i = 0; i < 3; ++i)
        {
          if (ReadBinaryStream(is, ints[i]))
          {
            bytesRead += sizeof(int);
          }
        }
        for (int i = 0; i < 6; ++i)
        {
          if (ReadBinaryStream(is, floats[i]))
          {
            bytesRead += sizeof(float);
          }
        }
        for (int i = 0; i < 2; ++i)
        {
          if (ReadBinaryStream(is, bs[i]))
          {
            bytesRead += sizeof(B);
          }
        }

        return bytesRead;
      }

      void ReadWhole(std::istream& is)
      {
        ReadBinaryStream(is, *this);
      }
    };

    TEST_METHOD(StreamUtils_ReadPiecewise)
    {
      A a;
      a.Init();

      std::stringstream io;
      a.WritePieceWise(io);

      io.seekg(0, SEEK_SET);

      A a2;
      a2.ReadPiecewise(io);

      Assert::IsTrue(memcmp(&a, &a2, sizeof(A)) == 0);
    }

    TEST_METHOD(StreamUtils_ReadWhole)
    {
      A a;
      a.Init();

      std::stringstream io;
      a.WritePieceWise(io);

      io.seekg(0, SEEK_SET);

      A a2;
      a2.ReadPiecewise(io);

      Assert::IsTrue(memcmp(&a, &a2, sizeof(A)) == 0);
    }

    // NOTE: mixing piecewise and whole reads will not work due to struct alignment.
  };
}