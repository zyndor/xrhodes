#include "gtest/gtest.h"
#include "FileLifeCycleManager.hpp"
#include "XR/File.hpp"
#include "XR/jsonutils.hpp"
#include "XR/JsonReader.hpp"
#include "XR/ScopeGuard.hpp"
#include "XR/debug.hpp"

namespace XR
{
  class JsonUtils: public ::testing::Test
  {
  public:
    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}

  private:
    FileLifeCycleManager  flcm;
  };

  TEST_F(JsonUtils, LoadJson)
  {
    auto json = LoadJSON("loadjson.json", 3, false);
    ASSERT_NE(json, nullptr);

    ASSERT_EQ(json->GetType(), JSON::OBJECT);
    ASSERT_NE(json->ToObject(), nullptr);
    ASSERT_EQ(json->GetNumChildren(), 4);

    auto intValue = json->GetChild("intValue");
    ASSERT_NE(intValue, nullptr);
    ASSERT_EQ(intValue->GetType(), JSON::VALUE);
    ASSERT_NE(intValue->ToValue(), nullptr);
    ASSERT_STREQ(intValue->GetValue(), "17753623");

    auto doubleValue = json->GetChild("doubleValue");
    ASSERT_NE(doubleValue, nullptr);
    ASSERT_EQ(doubleValue->GetType(), JSON::VALUE);
    ASSERT_NE(doubleValue->ToValue(), nullptr);
    ASSERT_STREQ(doubleValue->GetValue(), "0.27541829630000001");

    auto stringValue = json->GetChild("stringValue");
    ASSERT_NE(stringValue, nullptr);
    ASSERT_EQ(stringValue->GetType(), JSON::VALUE);
    ASSERT_NE(stringValue->ToValue(), nullptr);
    ASSERT_STREQ(stringValue->GetValue(), "lorem ipsum dolor sic amet e pluribus unum veni vidi vici");

    auto array = json->GetChild("array");
    ASSERT_NE(array, nullptr);
    ASSERT_EQ(array->GetType(), JSON::ARRAY);
    ASSERT_NE(array->ToArray(), nullptr);
    ASSERT_EQ(array->GetNumElements(), 6);

    // false is collapsed into 0
    ASSERT_STREQ(array->GetElement(0)->GetValue(), "0");

    auto object = array->GetElement(4);
    ASSERT_EQ(object->GetType(), JSON::OBJECT);
    ASSERT_STREQ(object->GetChild("name")->GetValue(), "four");
    ASSERT_STREQ(object->GetChild("value")->GetValue(), "4");

    array = array->GetElement(5);
    ASSERT_EQ(array->GetNumElements(), 3);

    // Max depth exceeded entities are empty.
    object = array->GetElement(2);
    ASSERT_NE(object, nullptr);
    ASSERT_EQ(object->GetType(), JSON::OBJECT);
    ASSERT_NE(object->ToObject(), nullptr);
    ASSERT_EQ(object->GetNumChildren(), 0);
  }
}
