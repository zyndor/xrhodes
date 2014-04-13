//
// Nuclear Heart Games
// XRhodes
//
// AnimationReader.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    04/06/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_ANIMATIONREADER_HPP
#define XR_ANIMATIONREADER_HPP

#include  <tinyxml.h>
#include  "types.hpp"
#include  "strings.hpp"
#include  "Parse.hpp"
#include  "Animation.hpp"

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
}

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

  typedef bool(*GetFrameDataCallback)(const char*, void*, Type&);

  // static
  static bool  Read(TiXmlElement* pXml, GetFrameDataCallback pGetFrameDataCb,
    void* pGetFrameDataCbData, AnimationType& anim);
};

//==============================================================================
/*
  implementation
*/
//==============================================================================
template  <class T>
bool  AnimationReader<T>::Read(TiXmlElement* pXml,
  GetFrameDataCallback pGetFrameDataCb, void* pGetFrameDataCbData,
  AnimationType& anim)
{
  XR_ASSERT(AnimationReader, pXml != 0);
  XR_ASSERT(AnimationReader, pGetFrameDataCb != 0);

  bool  success;
  // name
  const char* pName(pXml->Attribute(karAnimationTag[AnimationTag::NAME]));
  success = pName != 0;
  if(!success)
  {
    XR_TRACE(AnimationReader, ("Attribute '%s' is required in animation",
      karAnimationTag[AnimationTag::NAME]));
  }

  // resource name
  const char* pResourceName(0);
  if(success)
  {
    pResourceName = pXml->Attribute(karAnimationTag[AnimationTag::RESOURCE_NAME]);
    if(pResourceName == 0)
    {
      pResourceName = pName;  // default to name
    }

    XR_ASSERTMSG(AnimationReader, strlen(pResourceName) + strlen("_f###") <
      static_cast<size_t>(SString().capacity()),
      ("'%s' for animation is too long",
        karAnimationTag[AnimationTag::RESOURCE_NAME]));
  }

  // frame delay
  if(success)
  {
    success = ParseFloat(pXml->Attribute(karAnimationTag[AnimationTag::FRAME_DELAY]),
      anim.frameDelay);
    if(!success)
    {
      XR_TRACE(AnimationReader, ("Failed to parse '%s' for animation",
        karAnimationTag[AnimationTag::FRAME_DELAY]));
    }
  }

  // loop frame
  if(success)
  {
    int iTemp;
    if(Parse::Int(pXml->Attribute(karAnimationTag[AnimationTag::LOOP_FRAME]),
      iTemp))
    {
      anim.SetLoopFrame(iTemp);
    }
  }

  if(success)
  {
    SString name(pResourceName);
    name += "_f###";

    char* pWrite(strchr(name.c_str(), '#'));

    anim.frames.resize(kMaxAnimationFrames);

    int   numFrames(0);
    bool  gotFrame(false);
    do
    {
      snprintf(pWrite, 3, "%03d", numFrames + 1); // names are 1-based
        
      gotFrame = (*pGetFrameDataCb)(name.c_str(), pGetFrameDataCbData,
        anim.frames[numFrames]);
      if(gotFrame)
      {
        ++numFrames;
      }
    } while(gotFrame && numFrames < kMaxAnimationFrames);

    anim.frames.resize_optimised(numFrames);

    if(numFrames == 0)
    {
      XR_TRACE(AnimationReader,
        ("Animation '%s' has 0 frames (resource name: %s).", pName,
          pResourceName));
    }
  }
  return success;
}

} // XR

#endif  // XR_ANIMATIONREADER_HPP