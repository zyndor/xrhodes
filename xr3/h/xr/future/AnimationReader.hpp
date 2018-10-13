#ifndef XR_ANIMATIONREADER_HPP
#define XR_ANIMATIONREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "Parse.hpp"
#include "Animation.hpp"
#include "xr/strings/ParserCore.hpp"
#include "xr/types/fundamentals.hpp"
#include "tinyxml2.h"
#include <list>

namespace xr
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

extern const char*  kAnimationTags[AnimationTag::kCount];

//==============================================================================
template  <class T>
class AnimationReader
{
  XR_NONOBJECT_DECL(AnimationReader)

public:
  // types
  typedef T               Type;
  typedef Animation<Type> AnimationType;

  typedef bool(*GetFrameDataCallback)(const char* name, int index, void* data,
    Type& frame);

  // static
  static bool  Read(tinyxml2::XMLElement* xml, GetFrameDataCallback getFrameDataCb,
    void* getFrameDataCbData, AnimationType& anim);
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
bool  AnimationReader<T>::Read(tinyxml2::XMLElement* xml,
  GetFrameDataCallback getFrameDataCb, void* getFrameDataCbData,
  AnimationType& anim)
{
  XR_ASSERT(AnimationReader, xml != nullptr);
  XR_ASSERT(AnimationReader, getFrameDataCb != nullptr);

  bool  success;
  // name
  const char* name = xml->Attribute(kAnimationTags[AnimationTag::NAME]);
  success = name != nullptr;
  if (!success)
  {
    XR_TRACE(AnimationReader, ("Attribute '%s' is required in animation",
      kAnimationTags[AnimationTag::NAME]));
  }

  // resource name
  const char* resourceName = nullptr;
  if (success)
  {
    resourceName = xml->Attribute(kAnimationTags[AnimationTag::RESOURCE_NAME]);
    if (resourceName == nullptr)
    {
      resourceName = name;  // default to name
    }
  }

  // frame delay
  if (success)
  {
    success = Parse::Float(xml->Attribute(kAnimationTags[AnimationTag::FRAME_DELAY]),
      anim.frameDelay);
    if (!success)
    {
      XR_TRACE(AnimationReader, ("Failed to parse '%s' for animation",
        kAnimationTags[AnimationTag::FRAME_DELAY]));
    }
  }

  // loop frame
  if (success)
  {
    int iTemp;
    if (Parse::Int(xml->Attribute(kAnimationTags[AnimationTag::LOOP_FRAME]),
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
      gotFrame = (*getFrameDataCb)(resourceName, frames.size(),
        getFrameDataCbData, frame);
      if (gotFrame)
      {
        frames.push_back(frame);
      }
    } while (gotFrame && frames.size() < kMaxAnimationFrames);

    if (frames.empty())
    {
      XR_TRACE(AnimationReader,
        ("Animation '%s' has 0 frames (resource name: %s).", name,
          resourceName));
    }
    else
    {
      // have we got a comma separated list of frames as text?
      tinyxml2::XMLNode*  text(xml->FirstChild());
      while(!text)
      {
        if(text->ToText())
        {
          break;
        }
        text = text->NextSibling();
      }

      typename AnimationType::FrameVector  fv;
      fv.assign(frames.begin(), frames.end());

      if(text != nullptr && text->ToText())
      {
        ParserCore  parser;
        parser.SetBuffer(text->Value(), strlen(text->Value()));

        frames.clear();
        while(!parser.IsOver(parser.ExpectChar()))
        {
          int i = atoi(parser.GetChar()) - 1;
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

} // xr

#endif  // XR_ANIMATIONREADER_HPP
