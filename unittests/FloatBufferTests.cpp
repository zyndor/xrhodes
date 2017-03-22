#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/FloatBuffer.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(FloatBufferTests)
  {
  public:
    TEST_METHOD(FloatBuffer_GetSet)
    {
      FloatBuffer fb;

      const size_t kNumElements = 4;
      fb.SetBuffer<Vector3>(kNumElements);

      Assert::IsTrue(fb.GetElementSizeFloats() == Vector3::kNumComponents);
      Assert::IsTrue(fb.GetElementSizeBytes() == sizeof(Vector3));
      Assert::IsTrue(fb.GetNumElements() == kNumElements);

      fb.Set(0, Vector3::UnitX());
      fb.Get<Vector3>(1) = Vector3::UnitY() * 2.0f;
      fb.Get<Vector3>()[2] = Vector3::UnitZ() * 3.0f;
      auto v = Vector3::One();
      fb.Set(1, &v, 3);

      float arTest[] = { 1.0f, .0f, .0f, .0f, 2.0f, .0f, .0f, .0f, 3.0f, 1.0f, 1.0f, 1.0f };
      Assert::IsTrue(sizeof(arTest) == fb.GetNumElements() * fb.GetElementSizeBytes());
      Assert::IsTrue(std::memcmp(fb.GetRaw(), arTest, sizeof(arTest)) == 0);
    }

    TEST_METHOD(FloatBuffer_ForEach)
    {
      FloatBuffer fb;

      const size_t kNumElements = 4;
      fb.SetBuffer<Vector3>(kNumElements);

      fb.Set(0, Vector3::One());
      fb.Set(1, Vector3::UnitZ() * 2.0f);
      fb.Set(2, Vector3::UnitY() * 3.0f);
      fb.Set(3, Vector3::UnitX() * 4.0f);

      fb.ForEach<Vector3>([](Vector3& v) {
        v.x += 1.0f;
        v.y -= 1.0f;
        v.z *= 2.0f;
      });

      float arTest[] = { 2.0f, .0f, 2.0f, 1.0f, -1.0f, 4.0f, 1.0f, 2.0f, .0f, 5.0f, -1.0f, .0f };
      Assert::IsTrue(sizeof(arTest) == fb.GetNumElements() * fb.GetElementSizeBytes());
      Assert::IsTrue(std::memcmp(fb.GetRaw(), arTest, sizeof(arTest)) == 0);
    }

    TEST_METHOD(FloatBuffer_Adapt)
    {
      FloatBuffer fb;

      const size_t kNumElements = 4;
      fb.SetBuffer<Vector3>(kNumElements);

      fb.Set(0, Vector3::One());
      fb.Set(1, Vector3::UnitZ() * 2.0f);
      fb.Set(2, Vector3::UnitY() * 3.0f);
      fb.Set(3, Vector3::UnitX() * 4.0f);

      FloatBuffer fb2 = FloatBuffer::Adapt(fb, 1, 2);
      Assert::IsTrue(fb2.GetElementSizeFloats() == fb.GetElementSizeFloats());
      Assert::IsTrue(fb2.GetNumElements() == 2);

      fb2.Set<Vector3>(1, fb2.Get<Vector3>(1) * 5.0f - Vector3::One());
      auto v0 = fb.Get<Vector3>(2);
      auto v1 = Vector3::UnitY() * 15.0f - Vector3::One();
      Assert::IsTrue(v0.x == v1.x && v0.y == v1.y && v0.z == v1.z);
    }
  };
}