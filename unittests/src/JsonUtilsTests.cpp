//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "FileLifeCycleManager.hpp"
#include "xr/File.hpp"
#include "xr/jsonutils.hpp"
#include "xr/JsonReader.hpp"
#include "xr/memory/ScopeGuard.hpp"
#include "xr/debug.hpp"

using namespace xr;

namespace
{

class JsonUtils
{
private:
  FileLifeCycleManager  flcm;
};

XM_TEST_F(JsonUtils, LoadJson)
{
  auto json = LoadJSON("loadjson.json", 3, false);
  XM_ASSERT_NE(json, nullptr);

  XM_ASSERT_EQ(json->GetType(), JSON::OBJECT);
  XM_ASSERT_NE(json->ToObject(), nullptr);
  XM_ASSERT_EQ(json->GetNumChildren(), 4u);

  auto intValue = json->GetChild("intValue");
  XM_ASSERT_NE(intValue, nullptr);
  XM_ASSERT_EQ(intValue->GetType(), JSON::VALUE);
  XM_ASSERT_NE(intValue->ToValue(), nullptr);
  XM_ASSERT_STREQ(intValue->GetValue(), "17753623");

  auto doubleValue = json->GetChild("doubleValue");
  XM_ASSERT_NE(doubleValue, nullptr);
  XM_ASSERT_EQ(doubleValue->GetType(), JSON::VALUE);
  XM_ASSERT_NE(doubleValue->ToValue(), nullptr);
  XM_ASSERT_STREQ(doubleValue->GetValue(), "0.27541829630000001");

  auto stringValue = json->GetChild("stringValue");
  XM_ASSERT_NE(stringValue, nullptr);
  XM_ASSERT_EQ(stringValue->GetType(), JSON::VALUE);
  XM_ASSERT_NE(stringValue->ToValue(), nullptr);
  XM_ASSERT_STREQ(stringValue->GetValue(), "lorem ipsum dolor sic amet e pluribus unum veni vidi vici");

  auto array = json->GetChild("array");
  XM_ASSERT_NE(array, nullptr);
  XM_ASSERT_EQ(array->GetType(), JSON::ARRAY);
  XM_ASSERT_NE(array->ToArray(), nullptr);
  XM_ASSERT_EQ(array->GetNumElements(), 6u);

  // false is collapsed into 0
  XM_ASSERT_STREQ(array->GetElement(0)->GetValue(), "0");

  auto object = array->GetElement(4);
  XM_ASSERT_EQ(object->GetType(), JSON::OBJECT);
  XM_ASSERT_STREQ(object->GetChild("name")->GetValue(), "four");
  XM_ASSERT_STREQ(object->GetChild("value")->GetValue(), "4");

  array = array->GetElement(5);
  XM_ASSERT_EQ(array->GetNumElements(), 3u);

  // Max depth exceeded entities are empty.
  object = array->GetElement(2);
  XM_ASSERT_NE(object, nullptr);
  XM_ASSERT_EQ(object->GetType(), JSON::OBJECT);
  XM_ASSERT_NE(object->ToObject(), nullptr);
  XM_ASSERT_EQ(object->GetNumChildren(), 0u);
}

}
