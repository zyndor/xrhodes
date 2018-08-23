//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/XonBuildTree.hpp"
#include "xr/Queue.hpp"
#include "xr/stringutils.hpp"
#include <list>

namespace xr
{

//==============================================================================
struct XonTreeBuildState
{
  struct Frame
  {
    XonObject*            object;
    XonParser::String     keyCache;

    void AddElement(XonEntity& v)
    {
      if (keyCache.start)
      {
        XR_ASSERT(XonTreeBuildState, keyCache.length != -1);
        object->AddElement(std::string(keyCache.start, keyCache.length), v);
        keyCache.start = nullptr;  // consume key
      }
      else
      {
        object->AddElement(v);
      }
    }
  };

  static bool EventHandler(XonParser::Event e, XonParser::String const* string,
    void* userDataData)
  {
    return static_cast<XonTreeBuildState*>(userDataData)->HandleEvent(e, string);
  }

  XonParser                   parser;
  std::unique_ptr<XonObject>  root;

  std::list<Frame> stack;

  void AddElement(XonParser::String s)
  {
    std::vector<char> buffer;
    if (!s.isQuoted && strncmp(s.start, "null", s.length) == 0)
    {
      s.start = nullptr;
      s.length = -1;
    }
    else if(s.isQuoted && s.length > 0)
    {
      buffer.resize(s.length);
      size_t replacedLen;
      Replace(s.start, s.length, "\\\"", "\"", s.length, buffer.data(), replacedLen);
      s.start = buffer.data();
      s.length = replacedLen;
    }
    stack.back().AddElement(*new XonValue(s.start, s.length));
  }

  bool HandleEvent(XonParser::Event e, XonParser::String const* string)
  {
    switch (e)
    {
    case XonParser::Event::ObjectBegin:
      {
        auto object = new XonObject;
        const bool first = root == nullptr;
        if (first)
        {
          XR_ASSERT(XonTreeBuildState, !root);
          root.reset(object);
        }
        else
        {
          stack.back().AddElement(*object);
        }
        stack.push_back({ object });
      }
      break;

    case XonParser::Event::ObjectEnd:
      {
        XR_ASSERT(XonTreeBuildState, !stack.empty());
        stack.pop_back();
      }
      break;

    case XonParser::Event::Key:
      XR_ASSERT(XonTreeBuildState, string);
      stack.back().keyCache = *string;
      break;

    case XonParser::Event::Value:
      XR_ASSERT(XonTreeBuildState, string);
      AddElement(*string);
      break;
    }

    return true;
  }
};

//==============================================================================
XonObject* XonBuildTree(char const* string, size_t length, XonParser::State* outState)
{
  XonTreeBuildState  rs;
  bool success = rs.parser.Parse(string, length, XonTreeBuildState::EventHandler, &rs);

  if (outState)
  {
    *outState = rs.parser.GetState();
  }
  return success ? rs.root.release() : nullptr;
}

} // xr
