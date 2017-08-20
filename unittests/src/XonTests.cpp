#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/XonParser.hpp>
#include <XR/XonBuildTree.hpp>
#include <XR/FileBuffer.hpp>
#include <list>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(XonTests)
  {
  public:
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

    TEST_METHOD(Xon_ParseValid) // Parse a XON file that tries to capture all sorts of scenarios.
    {
      XR::FileBuffer  buffer;
      buffer.Open(DATA_PATH "/xontest1.xon", false);

      XR::XonParser parser;
      EventSequencer seq;
      bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), SimpleEventHandler, &seq);
      Assert::IsTrue(success);
      Assert::IsTrue(parser.GetState().pCurrent == buffer.CastData<char>() + buffer.GetSize());

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

      Assert::IsTrue(seq.events.size() == testEvents.size());
      auto iEvents = testEvents.begin();
      for (auto& i : seq.events)
      {
        Assert::IsTrue(i == *iEvents);
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

      Assert::IsTrue(seq.strings.size() == testStrings.size());
      auto iStrings = testStrings.begin();
      for (auto& i : seq.strings)
      {
        Assert::IsTrue(i.pStart == nullptr ? i.pStart == iStrings->first :
          strncmp(i.pStart, iStrings->first, i.length) == 0);
        Assert::IsTrue(i.isQuoted == iStrings->second);
        ++iStrings;
      }
    }

    static bool  XonNoopHandler(XonParser::Event, XonParser::String const*, void*)
    {
      return true;
    }

    TEST_METHOD(Xon_ParseInvalid)  // Parsing a number of invalid XON files. Should all fail.
    {
      char arBuffer[256];
      XR::XonParser parser;

      for (int i = 0; i < 5; ++i)
      {
        XR::FileBuffer  buffer;

        sprintf(arBuffer, "%s/invalid%d.xon", DATA_PATH, i + 1);
        buffer.Open(arBuffer, false);

        bool success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        Assert::IsFalse(success);
      }
    }

    TEST_METHOD(Xon_ParserReuse)  // The same parser instance can be reused; the state from one parse shall not affect the result of a subsequent parse.
    {
      XR::XonParser parser;
      bool success;

      for (int i = 0; i < 2; ++i)
      {
        XR::FileBuffer  buffer;
        buffer.Open(DATA_PATH "/xontest1.xon", false);
        success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        Assert::IsTrue(success);
        Assert::IsTrue(parser.GetState().pCurrent == buffer.CastData<char>() + buffer.GetSize());

        buffer.Open(DATA_PATH "/invalid1.xon", false);
        success = parser.Parse(buffer.CastData<char>(), buffer.GetSize(), XonNoopHandler, nullptr);
        Assert::IsFalse(success);
        Assert::IsTrue(parser.GetState().row == 4);
        Assert::IsTrue(parser.GetState().column == 1);
      }
    }

    TEST_METHOD(Xon_ReadValid) // Read xon and construct tree.
    {
      XR::FileBuffer  buffer;
      buffer.Open(DATA_PATH "/xontest1.xon", false);

      XonParser::State  readState;
      std::unique_ptr<XonObject> root(XonBuildTree(buffer.CastData<char>(), buffer.GetSize(), &readState));
      Assert::IsTrue(root != nullptr);
      Assert::IsTrue(readState.pCurrent == buffer.CastData<char>() + buffer.GetSize());

      Assert::IsTrue(root->GetNumElements() == 7);

      XonEntity& v0((*root)[0]);
      Assert::IsTrue(v0.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v0.GetValue() == std::string("value"));

      XonEntity& v1((*root)[1]);
      Assert::IsTrue(v1.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v1.GetValue() == std::string("value"));
      Assert::IsTrue(&v1 != &root->Find("key"));

      XonEntity& v2((*root)[2]);
      Assert::IsTrue(v2.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v2.GetValue() == std::string("value2"));
      Assert::IsTrue(&v2 == &root->Find("key"));

      XonEntity& v3((*root)[3]);
      Assert::IsTrue(v3.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v3.GetValue() == std::string("more values"));
      Assert::IsTrue(&v3 == &root->Find("more_keys"));

      XonEntity& v4((*root)[4]);
      Assert::IsTrue(v4.GetType() == XonEntity::Type::Object);
      Assert::IsTrue(v4.GetNumElements() == 7);

      XonEntity& v4_0(v4[0]);
      Assert::IsTrue(v4_0.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_0.GetValue() == std::string("object_value"));

      XonEntity& v4_1(v4[1]);
      Assert::IsTrue(v4_1.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_1.GetValue() == std::string("value"));
      Assert::IsTrue(&v4_1 == &v4.Find("key"));

      XonEntity& v4_2(v4[2]);
      Assert::IsTrue(v4_2.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_2.GetValue() == std::string("{}ther_va:ue"));
      Assert::IsTrue(&v4_2 == &v4.Find("{}ther key"));

      XonEntity& v4_3(v4[3]);
      Assert::IsTrue(v4_3.GetType() == XonEntity::Type::Object);
      Assert::IsTrue(v4_3.GetNumElements() == 3);
      Assert::IsTrue(&v4_3 == &v4.Find("a_nested_object"));

      XonEntity& v4_3_0(v4_3[0]);
      Assert::IsTrue(v4_3_0.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_3_0.GetValue() == std::string("true"));
      Assert::IsTrue(&v4_3_0 == &v4_3.Find("nested"));

      XonEntity& v4_3_1(v4_3[1]);
      Assert::IsTrue(v4_3_1.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_3_1.GetValue() == std::string("escaped \"quot\""));
      //Assert::IsTrue(v4_3_1.GetValue() == std::string("escaped \\\"quot\\\""));

      XonEntity& v4_3_2(v4_3[2]);
      Assert::IsTrue(v4_3_2.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_3_2.GetValue() == std::string(""));

      XonEntity& v4_4(v4[4]);
      Assert::IsTrue(v4_4.GetType() == XonEntity::Type::Object);
      Assert::IsTrue(v4_4.GetNumElements() == 0);

      XonEntity& v4_5(v4[5]);
      Assert::IsTrue(v4_5.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_5.GetValue() == std::string("another_value"));

      XonEntity& v4_6(v4[6]);
      Assert::IsTrue(v4_6.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v4_6.GetValue() == std::string("a_value_that's_\"quoted\""));

      XonEntity& v5((*root)[5]);
      Assert::IsTrue(v5.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v5.GetValue() == nullptr);

      XonEntity& v6((*root)[6]);
      Assert::IsTrue(v6.GetType() == XonEntity::Type::Value);
      Assert::IsTrue(v6.GetValue() == std::string("null"));
    }

    TEST_METHOD(Xon_ReadInvalid)  // Read a bunch of invalid xon files. All should fail.
    {
      char arBuffer[256];
      for (int i = 0; i < 5; ++i)
      {
        XR::FileBuffer  buffer;

        sprintf(arBuffer, "%s/invalid%d.xon", DATA_PATH, i + 1);
        buffer.Open(arBuffer, false);

        XonObject* pRoot = XonBuildTree(buffer.CastData<char>(), buffer.GetSize());
        Assert::IsTrue(!pRoot);
      }
    }

    void TestXonError(std::function<void()> fn, XonEntity::Exception::Type testType)
    {
      try
      {
        fn();
        Assert::Fail(L"Must throw.");
      }
      catch (XonEntity::Exception& x)
      {
        Logger::WriteMessage(x.what());
        Assert::IsTrue(x.type == testType);
      }
      catch (...)
      {
        Assert::Fail(L"Wrong exception type.");
      }
    }

    TEST_METHOD(Xon_Errors) // Test for entity errors.
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
      Assert::IsTrue(v0.GetType() == XonEntity::Type::Value);

      TestXonError([&v0]() { v0.GetNumElements(); }, XonEntity::Exception::Type::InvalidType);
      TestXonError([&v0]() { v0[0]; }, XonEntity::Exception::Type::InvalidType);
      TestXonError([&v0]() { v0.Find(""); }, XonEntity::Exception::Type::InvalidType);
    }
  };
}
