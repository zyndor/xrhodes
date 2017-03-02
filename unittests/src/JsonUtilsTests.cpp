#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/jsonutils.hpp>

#include <XR/JsonReader.hpp>
#include <XR/FileBuffer.hpp>
#include <XR/debug.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(JsonUtilsTests)
  {
  public:
    TEST_METHOD(JsonUtils_LoadJson)
    {
      auto json = LoadJSON("../../data/loadjson.json", 3, false);
      Assert::IsTrue(json);

    }
  };
}