#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/XonParser.hpp>
#include <XR/FileBuffer.hpp>
#include <list>

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
      buffer.Open(DATA_PATH "/xontest1.xon", "rb");

      XR::XonParser parser;
      EventSequencer seq;
      bool success = parser.Parse(buffer.GetData(), buffer.GetSize(), SimpleEventHandler, &seq);
      Assert::IsTrue(success);
      Assert::IsTrue(parser.GetState().pCurrent == buffer.GetData() + buffer.GetSize());

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
        buffer.Open(arBuffer, "rb");

        bool success = parser.Parse(buffer.GetData(), buffer.GetSize(), XonNoopHandler, nullptr);
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
        buffer.Open(DATA_PATH "/xontest1.xon", "rb");
        success = parser.Parse(buffer.GetData(), buffer.GetSize(), XonNoopHandler, nullptr);
        Assert::IsTrue(success);
        Assert::IsTrue(parser.GetState().pCurrent == buffer.GetData() + buffer.GetSize());

        buffer.Open(DATA_PATH "/invalid1.xon", "rb");
        success = parser.Parse(buffer.GetData(), buffer.GetSize(), XonNoopHandler, nullptr);
        Assert::IsFalse(success);
        Assert::IsTrue(parser.GetState().row == 4);
        Assert::IsTrue(parser.GetState().column == 1);
      }
    }
  };
}