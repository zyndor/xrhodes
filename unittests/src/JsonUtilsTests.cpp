#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/jsonutils.hpp>

#include <XR/JsonReader.hpp>
#include <XR/debug.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(JsonUtilsTests)
  {
  public:
    TEST_METHOD(JsonUtils_LoadJson)
    {
      auto json = LoadJSON(DATA_PATH "/loadjson.json", 3, false);
      Assert::IsTrue(json);

      Assert::IsTrue(json->GetType() == JSON::OBJECT);
      Assert::IsTrue(json->ToObject());
      Assert::AreEqual(json->GetNumChildren(), 4);

      auto intValue = json->GetChild("intValue");
      Assert::IsTrue(intValue);
      Assert::IsTrue(intValue->GetType() == JSON::VALUE);
      Assert::IsTrue(intValue->ToValue());
      Assert::IsTrue(std::strcmp(intValue->GetValue(), "17753623") == 0);

      auto doubleValue = json->GetChild("doubleValue");
      Assert::IsTrue(doubleValue);
      Assert::IsTrue(doubleValue->GetType() == JSON::VALUE);
      Assert::IsTrue(doubleValue->ToValue());
      Assert::IsTrue(std::strcmp(doubleValue->GetValue(), "0.27541829630000001") == 0);

      auto stringValue = json->GetChild("stringValue");
      Assert::IsTrue(stringValue);
      Assert::IsTrue(stringValue->GetType() == JSON::VALUE);
      Assert::IsTrue(stringValue->ToValue());
      Assert::IsTrue(std::strcmp(stringValue->GetValue(), "lorem ipsum dolor sic amet e pluribus unum veni vidi vici") == 0);

      auto array = json->GetChild("array");
      Assert::IsTrue(array);
      Assert::IsTrue(array->GetType() == JSON::ARRAY);
      Assert::IsTrue(array->ToArray());
      Assert::AreEqual(array->GetNumElements(), 6);

      // false is collapsed into 0
      Assert::IsTrue(std::strcmp(array->GetElement(0)->GetValue(), "0") == 0);

      auto object = array->GetElement(4);
      Assert::IsTrue(object->GetType() == JSON::OBJECT);
      Assert::IsTrue(std::strcmp(object->GetChild("name")->GetValue(), "four") == 0);
      Assert::IsTrue(std::strcmp(object->GetChild("value")->GetValue(), "4") == 0);

      array = array->GetElement(5);
      Assert::AreEqual(array->GetNumElements(), 3);

      // Max depth exceeded entities are empty.
      object = array->GetElement(2);
      Assert::IsTrue(object);
      Assert::IsTrue(object->GetType() == JSON::OBJECT);
      Assert::IsTrue(object->ToObject());
      Assert::AreEqual(object->GetNumChildren(), 0);
    }
  };
}
