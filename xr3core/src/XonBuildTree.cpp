//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/XonBuildTree.hpp>
#include <XR/Queue.hpp>
#include <XR/stringutils.hpp>
#include <list>

namespace XR
{

//==============================================================================
struct XonTreeBuildState
{
  struct Frame
  {
    XonObject*            pObject;
    XonParser::String     keyCache;

    void AddValue(XonEntity& v)
    {
      if (keyCache.pStart)
      {
        XR_ASSERT(XonTreeBuildState, keyCache.length != -1);
        pObject->AddValue(std::string(keyCache.pStart, keyCache.length), v);
        keyCache.pStart = nullptr;  // consume key
      }
      else
      {
        pObject->AddValue(v);
      }
    }
  };

  static bool EventHandler(XonParser::Event e, XonParser::String const* pString,
    void* pUserData)
  {
    return static_cast<XonTreeBuildState*>(pUserData)->HandleEvent(e, pString);
  }

  XonParser                   parser;
  std::unique_ptr<XonObject>  pRoot;

  std::list<Frame> stack;

  void AddValue(XonParser::String s)
  {
    std::vector<char> buffer;
    if (!s.isQuoted && strncmp(s.pStart, "null", s.length) == 0)
    {
      s.pStart = nullptr;
      s.length = -1;
    }
    else if(s.isQuoted && s.length > 0)
    {
      buffer.resize(s.length);
      size_t replacedLen;
      Replace(s.pStart, s.length, "\\\"", "\"", s.length, buffer.data(), replacedLen);
      s.pStart = buffer.data();
      s.length = replacedLen;
    }
    stack.back().AddValue(*new XonValue(s.pStart, s.length));
  }

  bool HandleEvent(XonParser::Event e, XonParser::String const* pString)
  {
    switch (e)
    {
    case XonParser::Event::ObjectBegin:
      {
        auto pObj = new XonObject;
        const bool first = pRoot == nullptr;
        if (first)
        {
          XR_ASSERT(XonTreeBuildState, !pRoot);
          pRoot.reset(pObj);
        }
        else
        {
          stack.back().AddValue(*pObj);
        }
        stack.push_back({ pObj });
      }
      break;

    case XonParser::Event::ObjectEnd:
      {
        XR_ASSERT(XonTreeBuildState, !stack.empty());
        stack.pop_back();
      }
      break;

    case XonParser::Event::Key:
      XR_ASSERT(XonTreeBuildState, pString);
      stack.back().keyCache = *pString;
      break;

    case XonParser::Event::Value:
      XR_ASSERT(XonTreeBuildState, pString);
      AddValue(*pString);
      break;
    }

    return true;
  }
};

//==============================================================================
XonObject* XonBuildTree(char const * pBuffer, size_t length, XonParser::State* pStateOut)
{
  XonTreeBuildState  rs;
  bool success = rs.parser.Parse(pBuffer, length, XonTreeBuildState::EventHandler, &rs);

  if (pStateOut)
  {
    *pStateOut = rs.parser.GetState();
  }
  return success ? rs.pRoot.release() : nullptr;
}

} // XR