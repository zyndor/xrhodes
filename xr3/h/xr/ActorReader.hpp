#ifndef XR_ACTORREADER_HPP
#define XR_ACTORREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/HardString.hpp"
#include "xr/fundamentals.hpp"
#include "AnimationReader.hpp"
#include "Actor.hpp"

namespace xr
{

//==============================================================================
namespace ActorTag
{
enum  type
{
  NAME,
  ACTION,
  kCount
};
}

//==============================================================================
extern const char*  kActorTags[ActorTag::kCount];

//==============================================================================
struct  ActorSessionDataCore
{
  const char* actorName;
  void*       getFrameDataCbData;
};

//==============================================================================
template  <class T>
class ActorReader
{
  XR_NONOBJECT_DECL(ActorReader)

public:
  // types
  typedef T           Type;
  typedef Actor<Type> ActorType;

  // static
  static bool Read(tinyxml2::XMLElement* xml,
    typename AnimationReader<Type>::GetFrameDataCallback getFrameDataCb,
    void* getFrameDataCbData, bool clearActions, ActorType& actor);

private:
  // types
  struct  SessionData:  ActorSessionDataCore
  {
    typename AnimationReader<T>::GetFrameDataCallback  getFrameDataCb;
  };

  // static
  static bool GetFrameDataCallback(const char* name, int index, void* data,
    Type& frame);
};

//==============================================================================
// implementation
//==============================================================================
template  <class Type>
bool ActorReader<Type>::Read(tinyxml2::XMLElement* xml,
  typename AnimationReader<Type>::GetFrameDataCallback getFrameDataCb,
  void* getFrameDataCbData, bool clearActions, Actor<Type>& actor)
{
  XR_ASSERT(ActorReader, xml != nullptr);
  if (clearActions)
  {
    actor.actions.clear();
  }

  const char* name = xml->Attribute(kActorTags[ActorTag::NAME]);
  bool  success(name != nullptr);
  if (!success)
  {
    XR_TRACE(ActorReader, ("Attribute '%s' is required in actor.",
      kActorTags[ActorTag::NAME]));
  }

  if (success)
  {
    xml = xml->FirstChildElement(kActorTags[ActorTag::ACTION]);

    SessionData sd;
    sd.actorName = name;
    sd.getFrameDataCbData = getFrameDataCbData;
    sd.getFrameDataCb = getFrameDataCb;

    while (xml)
    {
      const char* actionName = xml->Attribute(kAnimationTags[AnimationTag::NAME]);
      success = actionName != nullptr;
      if (!success)
      {
        XR_TRACE(ActorReader,
          ("Attribute '%s' is required in action in actor %s.",
          kAnimationTags[AnimationTag::NAME], name));
        break;
      }

      if (success)
      {
        const uint32_t hash = xr::Hash::String(actionName);
        if (actor.actions.find(hash) != actor.actions.end())
        {
          XR_TRACE(ActorReader,
            ("Action named '%s' already exists in actor '%s' and will be overwritten.",
              actionName, name));
        }

        success = AnimationReader<Type>::Read(xml, GetFrameDataCallback,
          &sd, actor.actions[hash]);
      }
      xml = xml->NextSiblingElement(kActorTags[ActorTag::ACTION]);
    }
  }
  return success;
}

//==============================================================================
template  <class Type>
bool ActorReader<Type>::GetFrameDataCallback(const char* name, int index,
      void* data, Type& frame)
{
  XR_ASSERT(ActorReader, name != nullptr);
  XR_ASSERT(ActorReader, data != nullptr);
  SessionData*  sessionData = static_cast<SessionData*>(data);

  HardString<128> hsName(sessionData->actorName);
  hsName += "_";
  hsName += name;

  return (*sessionData->getFrameDataCb)(hsName.c_str(), index,
    sessionData->getFrameDataCbData, frame);
}

} // xr

#endif  //XR_ACTORREADER_HPP
