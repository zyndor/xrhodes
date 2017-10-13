#include "gtest/gtest.h"
#include "XR/XonParser.hpp"
#include "XR/XonBuildTree.hpp"
#include "XR/FileBuffer.hpp"
#include <list>
#include <functional>

namespace XR
{
    struct EventSequencer
    {
      std::list<XonParser::Event>   events;
      std::list<XonParser::String>  strings;
    };

    static bool  SimpleEventHandler(XonParser::Event e, XonParser::String const* pString, void* pUser)
    {
      EventSequencer& seq(*static_cast<EventSequencer*>(pUser));
      seq.events.push_back(e);
      if (pString)
      {
        XonParser::String str(*pString);
        seq.strings.push_back(str);
      }
      return true;
    }

    TEST(Xon, ParseValid) // Parse a XON file that tries to capture all sorts of scenarios.
    {
      XR::FileBuffer  buffer;
      buffer.Open(DATA_PATH "/xontest1.xon", false);

      XR::XonParser parser;
      EventSequencer seq;
      bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), SimpleEventHandler, &seq);
      ASSERT_TRUE(success);
      ASSERT_EQ(parser.GetState().pCurrent, buffer.CastData<char>() + buffer.GetSize());

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
        ASSERT_TRUE(i.pStart == nullptr ? i.pStart == iStrings->first :
          strncmp(i.pStart, iStrings->first, i.length) == 0);
        ASSERT_EQ(i.isQuoted, iStrings->second);
        ++iStrings;
      }
    }

    static bool  XonNoopHandler(XonParser::Event, XonParser::String const*, void*)
    {
      return true;
    }

    TEST(Xon, ParseInvalid)  // Parsing a number of invalid XON files. Should all fail.
    {
      char arBuffer[256];
      XR::XonParser parser;

      for (int i = 0; i < 5; ++i)
      {
        XR::FileBuffer  buffer;

        sprintf(arBuffer, "%s/invalid%d.xon", DATA_PATH, i + 1);
        buffer.Open(arBuffer, false);

        bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        ASSERT_FALSE(success);
      }
    }

    TEST(Xon, ParserReuse)  // The same parser instance can be reused; the state from one parse shall not affect the result of a subsequent parse.
    {
      XR::XonParser parser;
      bool success;

      for (int i = 0; i < 2; ++i)
      {
        XR::FileBuffer  buffer;
        buffer.Open(DATA_PATH "/xontest1.xon", false);
        success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        ASSERT_TRUE(success);
        ASSERT_EQ(parser.GetState().pCurrent, buffer.CastData<char>() + buffer.GetSize());

        buffer.Open(DATA_PATH "/invalid1.xon", false);
        success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        ASSERT_FALSE(success);
        ASSERT_EQ(parser.GetState().row, 4);
        ASSERT_EQ(parser.GetState().column, 1);
      }
    }

    TEST(Xon, ReadValid) // Read xon and construct tree.
    {
      XR::FileBuffer  buffer;
      buffer.Open(DATA_PATH "/xontest1.xon", false);

      XonParser::State  readState;
      std::unique_ptr<XonObject> root(XonBuildTree(buffer.CastData<char>(), buffer.GetSize(), &readState));
      ASSERT_NE(root, nullptr);
      ASSERT_EQ(readState.pCurrent, buffer.CastData<char>() + buffer.GetSize());

      ASSERT_EQ(root->GetNumElements(), 7);

      XonEntity& v0((*root)[0]);
      ASSERT_EQ(v0.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v0.GetValue(), std::string("value"));

      XonEntity& v1((*root)[1]);
      ASSERT_EQ(v1.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v1.GetValue(), std::string("value"));
      ASSERT_NE(&v1, &root->Find("key"));

      XonEntity& v2((*root)[2]);
      ASSERT_EQ(v2.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v2.GetValue(), std::string("value2"));
      ASSERT_EQ(&v2, &root->Find("key"));

      XonEntity& v3((*root)[3]);
      ASSERT_EQ(v3.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v3.GetValue(), std::string("more values"));
      ASSERT_EQ(&v3, &root->Find("more_keys"));

      XonEntity& v4((*root)[4]);
      ASSERT_EQ(v4.GetType(), XonEntity::Type::Object);
      ASSERT_EQ(v4.GetNumElements(), 7);

      XonEntity& v4_0(v4[0]);
      ASSERT_EQ(v4_0.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_0.GetValue(), std::string("object_value"));

      XonEntity& v4_1(v4[1]);
      ASSERT_EQ(v4_1.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_1.GetValue(), std::string("value"));
      ASSERT_EQ(&v4_1, &v4.Find("key"));

      XonEntity& v4_2(v4[2]);
      ASSERT_EQ(v4_2.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_2.GetValue(), std::string("{}ther_va:ue"));
      ASSERT_EQ(&v4_2, &v4.Find("{}ther key"));

      XonEntity& v4_3(v4[3]);
      ASSERT_EQ(v4_3.GetType(), XonEntity::Type::Object);
      ASSERT_EQ(v4_3.GetNumElements(), 3);
      ASSERT_EQ(&v4_3, &v4.Find("a_nested_object"));

      XonEntity& v4_3_0(v4_3[0]);
      ASSERT_EQ(v4_3_0.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_3_0.GetValue(), std::string("true"));
      ASSERT_EQ(&v4_3_0, &v4_3.Find("nested"));

      XonEntity& v4_3_1(v4_3[1]);
      ASSERT_EQ(v4_3_1.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_3_1.GetValue(), std::string("escaped \"quot\""));
      //ASSERT_EQ(v4_3_1.GetValue(), std::string("escaped \\\"quot\\\""));

      XonEntity& v4_3_2(v4_3[2]);
      ASSERT_EQ(v4_3_2.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_3_2.GetValue(), std::string(""));

      XonEntity& v4_4(v4[4]);
      ASSERT_EQ(v4_4.GetType(), XonEntity::Type::Object);
      ASSERT_EQ(v4_4.GetNumElements(), 0);

      XonEntity& v4_5(v4[5]);
      ASSERT_EQ(v4_5.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_5.GetValue(), std::string("another_value"));

      XonEntity& v4_6(v4[6]);
      ASSERT_EQ(v4_6.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v4_6.GetValue(), std::string("a_value_that's_\"quoted\""));

      XonEntity& v5((*root)[5]);
      ASSERT_EQ(v5.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v5.GetValue(), nullptr);

      XonEntity& v6((*root)[6]);
      ASSERT_EQ(v6.GetType(), XonEntity::Type::Value);
      ASSERT_EQ(v6.GetValue(), std::string("null"));
    }

    TEST(Xon, ReadInvalid)  // Read a bunch of invalid xon files. All should fail.
    {
      char arBuffer[256];
      for (int i = 0; i < 5; ++i)
      {
        XR::FileBuffer  buffer;

        sprintf(arBuffer, "%s/invalid%d.xon", DATA_PATH, i + 1);
        buffer.Open(arBuffer, false);

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
        XR_TRACE(XonTests, (x.what()));
        ASSERT_EQ(x.type, testType);
      }
      catch (...)
      {
        ASSERT_FALSE(true);//, "Wrong exception type.");
      }
    }

    TEST(Xon, Errors) // Test for entity errors.
    {
      XR::FileBuffer  buffer;
      buffer.Open(DATA_PATH "/xontest1.xon", false);

      XonParser::State  readState;
      std::unique_ptr<XonObject> root(XonBuildTree(buffer.CastData<char>(), buffer.GetSize(), &readState));

      // index out of bounds
      TestXonError([&root]() { (*root)[-1]; }, XonEntity::Exception::Type::IndexOutOfBounds);
      TestXonError([&root]() { (*root)[root->GetNumElements()]; }, XonEntity::Exception::Type::IndexOutOfBounds);

      // invalid key
      TestXonError([&root]() { root->Find("thisKeyD3finitel4Doesn't_exist"); }, XonEntity::Exception::Type::InvalidKey);

      // invalid type
      TestXonError([&root]() { root->GetValue(); }, XonEntity::Exception::Type::InvalidType);

      XonEntity& v0((*root)[0]);
      ASSERT_EQ(v0.GetType(), XonEntity::Type::Value);

      TestXonError([&v0]() { v0.GetNumElements(); }, XonEntity::Exception::Type::InvalidType);
      TestXonError([&v0]() { v0[0]; }, XonEntity::Exception::Type::InvalidType);
      TestXonError([&v0]() { v0.Find(""); }, XonEntity::Exception::Type::InvalidType);
    }
}
