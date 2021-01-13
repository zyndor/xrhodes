//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "FileLifeCycleManager.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/xon/XonParser.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include <list>
#include <functional>

using namespace xr;

namespace
{

class Xon: public ::testing::Test
{
public:
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}

private:
  FileLifeCycleManager  flcm;
};

struct EventSequencer
{
  std::list<XonParser::Event>   events;
  std::list<XonParser::String>  strings;
};

static bool  SimpleEventHandler(XonParser::Event e, XonParser::String const* pString, void* pUser)
{
  EventSequencer& seq = *static_cast<EventSequencer*>(pUser);
  seq.events.push_back(e);
  if (pString)
  {
    XonParser::String str(*pString);
    seq.strings.push_back(str);
  }
  return true;
}

TEST_F(Xon, ParseValid) // Parse a XON file that tries to capture all sorts of scenarios.
{
  xr::FileBuffer  buffer;
  buffer.Open("xontest1.xon");

  xr::XonParser parser;
  EventSequencer seq;
  bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), SimpleEventHandler, &seq);
  ASSERT_TRUE(success);
  ASSERT_EQ(parser.GetState().cursor, buffer.CastData<char>() + buffer.GetSize());

  std::list<XonParser::Event> testEvents = {
    XonParser::Event::ObjectBegin,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::ObjectBegin,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::Key,
    XonParser::Event::ObjectBegin,
    XonParser::Event::Key,
    XonParser::Event::Value,
    XonParser::Event::Value,
    XonParser::Event::Value,
    XonParser::Event::ObjectEnd,
    XonParser::Event::ObjectBegin,
    XonParser::Event::ObjectEnd,
    XonParser::Event::Value,
    XonParser::Event::Value,
    XonParser::Event::ObjectEnd,
    XonParser::Event::Value,
    XonParser::Event::Value,
    XonParser::Event::ObjectEnd,
  };

  ASSERT_EQ(seq.events.size(), testEvents.size());
  auto iEvents = testEvents.begin();
  for (auto& i : seq.events)
  {
    ASSERT_EQ(i, *iEvents);
    ++iEvents;
  }

  // string values and quotedness
  std::list<std::pair<const char*, bool>> testStrings = {
    { "value", false },
    { "key", false },
    { "value", false },
    { "key", true },
    { "value2", false },
    { "more_keys", false },
    { "more values", true },
    { "object_value", false },
    { "key", false },
    { "value", false },
    { "{}ther key", true },
    { "{}ther_va:ue", true },
    { "a_nested_object", false },
    { "nested", false },
    { "true", false },
    { "escaped \\\"quot\\\"", true },
    { "", true },
    { "another_value", false },
    { "a_value_that's_\"quoted\"", false },
    { "null", false },
    { "null", true },
  };

  ASSERT_EQ(seq.strings.size(), testStrings.size());
  auto iStrings = testStrings.begin();
  for (auto& i : seq.strings)
  {
    ASSERT_TRUE(i.start == nullptr ? i.start == iStrings->first :
      strncmp(i.start, iStrings->first, i.length) == 0);
    ASSERT_EQ(i.isQuoted, iStrings->second);
    ++iStrings;
  }
}

static bool  XonNoopHandler(XonParser::Event, XonParser::String const*, void*)
{
  return true;
}

TEST_F(Xon, ParseInvalid)  // Parsing a number of invalid XON files. Should all fail.
{
  char arBuffer[256];
  xr::XonParser parser;

  for (int i = 0; i < 5; ++i)
  {
    xr::FileBuffer  buffer;

    sprintf(arBuffer, "invalid%d.xon", i + 1);
    buffer.Open(arBuffer);

    bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
    ASSERT_FALSE(success);
  }
}

TEST_F(Xon, ParserReuse)  // The same parser instance can be reused; the state from one parse shall not affect the result of a subsequent parse.
{
  xr::XonParser parser;
  bool success;

  for (int i = 0; i < 2; ++i)
  {
    xr::FileBuffer  buffer;
    buffer.Open("xontest1.xon");
    success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
    ASSERT_TRUE(success);
    ASSERT_EQ(parser.GetState().cursor, buffer.CastData<char>() + buffer.GetSize());

    buffer.Open("invalid1.xon");
    success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
    ASSERT_FALSE(success);
    ASSERT_EQ(parser.GetState().row, 4);
    ASSERT_EQ(parser.GetState().column, 1);
  }
}

TEST_F(Xon, ReadValid) // Read xon and construct tree.
{
  xr::FileBuffer  buffer;
  buffer.Open("xontest1.xon");

  XonParser::State  readState;
  std::unique_ptr<XonObject> root(XonBuildTree(buffer.CastData<char>(), buffer.GetSize(), &readState));
  ASSERT_NE(root, nullptr);
  ASSERT_EQ(readState.cursor, buffer.CastData<char>() + buffer.GetSize());

  ASSERT_EQ(root->GetNumElements(), 7);

  std::vector<std::string> keys;
  root->GetKeys(keys);
  ASSERT_EQ(keys.size(), 2);
  // note: lexicographic order -- happens to be the same order they're defined.
  ASSERT_STREQ(keys[0].c_str(), "key");
  ASSERT_STREQ(keys[1].c_str(), "more_keys");

  XonEntity& v0 = (*root)[0];
  ASSERT_EQ(v0.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v0.ToValue().GetString(), std::string("value"));
  ASSERT_EQ(v0.ToValue().GetLength(), 5u);

  XonEntity& v1 = (*root)[1];
  ASSERT_EQ(v1.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v1.ToValue().GetString(), std::string("value"));
  ASSERT_EQ(v1.ToValue().GetLength(), 5u);
  ASSERT_NE(&v1, &root->Get("key"));

  XonEntity& v2 = (*root)[2];
  ASSERT_EQ(v2.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v2.ToValue().GetString(), std::string("value2"));
  ASSERT_EQ(v2.ToValue().GetLength(), 6u);
  ASSERT_EQ(&v2, &root->Get("key"));

  XonEntity& v3 = (*root)[3];
  ASSERT_EQ(v3.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v3.ToValue().GetString(), std::string("more values"));
  ASSERT_EQ(v3.ToValue().GetLength(), 11u);
  ASSERT_EQ(&v3, &root->Get("more_keys"));

  XonEntity& v4 = (*root)[4];
  ASSERT_EQ(v4.GetType(), XonEntity::Type::Object);
  ASSERT_EQ(v4.ToObject().GetNumElements(), 7);

  static_cast<XonObject&>(v4).GetKeys(keys);
  ASSERT_EQ(keys.size(), 3);
  // note: lexicographic order.
  ASSERT_STREQ(keys[0].c_str(), "a_nested_object");
  ASSERT_STREQ(keys[1].c_str(), "key");
  ASSERT_STREQ(keys[2].c_str(), "{}ther key");

  XonObject& v4Object = v4.ToObject();
  XonEntity& v4_0 = v4Object[0];
  ASSERT_EQ(v4_0.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_0.ToValue().GetString(), std::string("object_value"));
  ASSERT_EQ(v4_0.ToValue().GetLength(), 12u);

  XonEntity& v4_1 = v4Object[1];
  ASSERT_EQ(v4_1.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_1.ToValue().GetString(), std::string("value"));
  ASSERT_EQ(v4_1.ToValue().GetLength(), 5u);
  ASSERT_EQ(&v4_1, &v4Object.Get("key"));

  XonEntity& v4_2 = v4Object[2];
  ASSERT_EQ(v4_2.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_2.ToValue().GetString(), std::string("{}ther_va:ue"));
  ASSERT_EQ(v4_2.ToValue().GetLength(), 12u);
  ASSERT_EQ(&v4_2, &v4Object.Get("{}ther key"));

  XonEntity& v4_3 = v4Object[3];
  ASSERT_EQ(v4_3.GetType(), XonEntity::Type::Object);
  ASSERT_EQ(v4_3.ToObject().GetNumElements(), 3);
  ASSERT_EQ(&v4_3, &v4Object.Get("a_nested_object"));

  XonObject& v4_3Object = v4_3.ToObject();
  XonEntity& v4_3_0 = v4_3Object[0];
  ASSERT_EQ(v4_3_0.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_3_0.ToValue().GetString(), std::string("true"));
  ASSERT_EQ(&v4_3_0, &v4_3.ToObject().Get("nested"));

  XonEntity& v4_3_1 = v4_3Object[1];
  ASSERT_EQ(v4_3_1.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_3_1.ToValue().GetString(), std::string("escaped \"quot\""));
  ASSERT_EQ(v4_3_1.ToValue().GetLength(), 14u);
  //ASSERT_EQ(v4_3_1.GetString(), std::string("escaped \\\"quot\\\""));

  XonEntity& v4_3_2 = v4_3Object[2];
  ASSERT_EQ(v4_3_2.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_3_2.ToValue().GetString(), std::string(""));
  ASSERT_EQ(v4_3_2.ToValue().GetLength(), 0);

  XonEntity& v4_4 = v4Object[4];
  ASSERT_EQ(v4_4.GetType(), XonEntity::Type::Object);
  ASSERT_EQ(v4_4.ToObject().GetNumElements(), 0);

  XonEntity& v4_5 = v4Object[5];
  ASSERT_EQ(v4_5.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_5.ToValue().GetString(), std::string("another_value"));
  ASSERT_EQ(v4_5.ToValue().GetLength(), 13u);

  XonEntity& v4_6 = v4Object[6];
  ASSERT_EQ(v4_6.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v4_6.ToValue().GetString(), std::string("a_value_that's_\"quoted\""));
  ASSERT_EQ(v4_6.ToValue().GetLength(), 23u);

  XonEntity& v5 = (*root)[5];
  ASSERT_EQ(v5.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v5.ToValue().GetString(), nullptr);
  ASSERT_EQ(v5.ToValue().GetLength(), 0);

  XonEntity& v6 = (*root)[6];
  ASSERT_EQ(v6.GetType(), XonEntity::Type::Value);
  ASSERT_EQ(v6.ToValue().GetString(), std::string("null"));
  ASSERT_EQ(v6.ToValue().GetLength(), 4u);
}

TEST_F(Xon, ReadInvalid)  // Read a bunch of invalid xon files. All should fail.
{
  char arBuffer[256];
  for (int i = 0; i < 5; ++i)
  {
    xr::FileBuffer  buffer;

    sprintf(arBuffer, "invalid%d.xon", i + 1);
    buffer.Open(arBuffer);

    XonObject* pRoot = XonBuildTree(buffer.CastData<char>(), buffer.GetSize());
    ASSERT_EQ(pRoot, nullptr);
  }
}

void TestXonError(std::function<void()> fn, XonEntity::Exception::Type testType)
{
  ASSERT_THROW(fn(), XonEntity::Exception);
  try
  {
    fn();
    ASSERT_FALSE(true);//, "Must throw.");
  }
  catch (XonEntity::Exception& x)
  {
    XR_TRACE(Xon, (x.what()));
    ASSERT_EQ(x.type, testType);
  }
  catch (...)
  {
    ASSERT_FALSE(true);//, "Wrong exception type.");
  }
}

TEST_F(Xon, Errors) // Test for entity errors.
{
  xr::FileBuffer  buffer;
  buffer.Open("xontest1.xon");

  XonParser::State  readState;
  std::unique_ptr<XonObject> root(XonBuildTree(buffer.CastData<char>(), buffer.GetSize(), &readState));

  // index out of bounds
  TestXonError([&root]() { (*root)[-1]; }, XonEntity::Exception::Type::IndexOutOfBounds);
  TestXonError([&root]() { (*root)[root->GetNumElements()]; }, XonEntity::Exception::Type::IndexOutOfBounds);

  // invalid key
  TestXonError([&root]() { root->Get("thisKeyD3finitel4Doesn't_exist"); }, XonEntity::Exception::Type::InvalidKey);

  // invalid type
  XonEntity& v0 = (*root)[0];
  ASSERT_EQ(v0.GetType(), XonEntity::Type::Value);

  TestXonError([&v0]() { v0.ToObject(); }, XonEntity::Exception::Type::InvalidType);
}

}
