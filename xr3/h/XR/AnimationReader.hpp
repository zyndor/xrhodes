#ifndef XR_ANIMATIONREADER_HPP
#define XR_ANIMATIONREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Parse.hpp"
#include "Animation.hpp"
#include "XR/ParserCore.hpp"
#include "XR/fundamentals.hpp"
#include "tinyxml2.h"
#include <list>

namespace XR
{
//==============================================================================
namespace AnimationTag
{
enum  type
{
  NAME,
  RESOURCE_NAME,  // overrides name
  FRAME_DELAY,
  LOOP_FRAME, // optional
  kCount
};
} // AnimationTag

//==============================================================================
extern const int    kMaxAnimationFrames;

extern const char*  karAnimationTag[AnimationTag::kCount];

//==============================================================================
template  <class T>
class AnimationReader
{
  XR_NONOBJECT_DECL(AnimationReader)

public:
  // types
  typedef T               Type;
  typedef Animation<Type> AnimationType;

  typedef bool(*GetFrameDataCallback)(const char* pName, int index, void* pData,
    Type& frame);

  // static
  static bool  Read(tinyxml2::XMLElement* pXml, GetFrameDataCallback pGetFrameDataCb,
    void* pGetFrameDataCbData, AnimationType& anim);
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
bool  AnimationReader<T>::Read(tinyxml2::XMLElement* pXml,
  GetFrameDataCallback pGetFrameDataCb, void* pGetFrameDataCbData,
  AnimationType& anim)
{
  XR_ASSERT(AnimationReader, pXml != 0);
  XR_ASSERT(AnimationReader, pGetFrameDataCb != 0);

  bool  success;
  // name
  const char* pName(pXml->Attribute(karAnimationTag[AnimationTag::NAME]));
  success = pName != 0;
  if (!success)
  {
    XR_TRACE(AnimationReader, ("Attribute '%s' is required in animation",
      karAnimationTag[AnimationTag::NAME]));
  }

  // resource name
  const char* pResourceName(0);
  if (success)
  {
    pResourceName = pXml->Attribute(karAnimationTag[AnimationTag::RESOURCE_NAME]);
    if (pResourceName == 0)
    {
      pResourceName = pName;  // default to name
    }
  }

  // frame delay
  if (success)
  {
    success = Parse::Float(pXml->Attribute(karAnimationTag[AnimationTag::FRAME_DELAY]),
      anim.frameDelay);
    if (!success)
    {
      XR_TRACE(AnimationReader, ("Failed to parse '%s' for animation",
        karAnimationTag[AnimationTag::FRAME_DELAY]));
    }
  }

  // loop frame
  if (success)
  {
    int iTemp;
    if (Parse::Int(pXml->Attribute(karAnimationTag[AnimationTag::LOOP_FRAME]),
      iTemp))
    {
      anim.SetLoopFrame(iTemp);
    }
  }

  if (success)
  {
    typedef std::list<Type> FrameList;
    FrameList frames;
    Type      frame;

    bool  gotFrame(false);
    do
    {
      gotFrame = (*pGetFrameDataCb)(pResourceName, frames.size(),
        pGetFrameDataCbData, frame);
      if (gotFrame)
      {
        frames.push_back(frame);
      }
    } while (gotFrame && frames.size() < kMaxAnimationFrames);
    
    if (frames.empty())
    {
      XR_TRACE(AnimationReader,
        ("Animation '%s' has 0 frames (resource name: %s).", pName,
          pResourceName));
    }
    else
    {
      // have we got a comma separated list of frames as text?
      tinyxml2::XMLNode*  pText(pXml->FirstChild());
      while(!pText)
      {
        if(pText->ToText())
        {
          break;
        }
        pText = pText->NextSibling();
      }

      typename AnimationType::FrameVector  fv;
      fv.assign(frames.begin(), frames.end());

      if(pText != nullptr && pText->ToText())
      {
        ParserCore  parser;
        parser.SetBuffer(pText->Value(), strlen(pText->Value()));

        frames.clear();
        while(!parser.IsOver(parser.ExpectChar()))
        {
          int i(atoi(parser.GetChar()) - 1);
          XR_ASSERT(AnimationReader, i >= 0 && i < fv.size());

          frames.push_back(fv[i]);

          if(parser.IsOver(parser.RequireChar(',')) ||
            parser.IsOver(parser.SkipChar()))
          {
            break;
          }
        }

        anim.frames.assign(frames.begin(), frames.end());
      }
      else
      {
        // just copy the frames neat
        anim.frames.swap(fv);
      }
    }
  }
  return success;
}

} // XR

#endif  // XR_ANIMATIONREADER_HPP
