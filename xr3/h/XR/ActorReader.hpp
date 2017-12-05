#ifndef XR_ACTORREADER_HPP
#define XR_ACTORREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "AnimationReader.hpp"
#include "Actor.hpp"
#include "XR/HardString.hpp"
#include "XR/fundamentals.hpp"

namespace XR
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
extern const char*  karActorTag[ActorTag::kCount];

//==============================================================================
struct  ActorSessionDataCore
{
  const char* pActorName;
  void*       pGetFrameDataCbData;
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
  static bool Read(tinyxml2::XMLElement* pXml,
    typename AnimationReader<Type>::GetFrameDataCallback pGetFrameDataCb,
    void* pGetFrameDataCbData, bool clearActions, ActorType& actor);
  
private:
  // types
  struct  SessionData:  ActorSessionDataCore
  {
    typename AnimationReader<T>::GetFrameDataCallback  pGetFrameDataCb;
  };

  // static
  static bool GetFrameDataCallback(const char* pName, int index, void* pData,
    Type& frame);
};

//==============================================================================
// implementation
//==============================================================================
template  <class Type>
bool ActorReader<Type>::Read(tinyxml2::XMLElement* pXml,
  typename AnimationReader<Type>::GetFrameDataCallback pGetFrameDataCb,
  void* pGetFrameDataCbData, bool clearActions, Actor<Type>& actor)
{
  XR_ASSERT(ActorReader, pXml != 0);
  if (clearActions)
  {
    actor.actions.clear();
  }

  const char* pName(pXml->Attribute(karActorTag[ActorTag::NAME]));
  bool  success(pName != 0);
  if (!success)
  {
    XR_TRACE(ActorReader, ("Attribute '%s' is required in actor.",
      karActorTag[ActorTag::NAME]));
  }

  if (success)
  {
    pXml = pXml->FirstChildElement(karActorTag[ActorTag::ACTION]);

    SessionData sd;
    sd.pActorName = pName;
    sd.pGetFrameDataCbData = pGetFrameDataCbData;
    sd.pGetFrameDataCb = pGetFrameDataCb;

    while (pXml != 0)
    {
      const char* pActionName(pXml->Attribute(karAnimationTag[AnimationTag::NAME]));
      success = pActionName != 0;
      if (!success)
      {
        XR_TRACE(ActorReader,
          ("Attribute '%s' is required in action in actor %s.",
          karAnimationTag[AnimationTag::NAME], pName));
        break;
      }

      if (success)
      {
        const uint32_t  hash(XR::Hash::String(pActionName));
        if (actor.actions.find(hash) != actor.actions.end())
        {
          XR_TRACE(ActorReader,
            ("Action named '%s' already exists in actor '%s' and will be overwritten.",
              pActionName, pName));
        }

        success = AnimationReader<Type>::Read(pXml, GetFrameDataCallback,
          &sd, actor.actions[hash]);
      }
      pXml = pXml->NextSiblingElement(karActorTag[ActorTag::ACTION]);
    }
  }
  return success;
}

//==============================================================================
template  <class Type>
bool ActorReader<Type>::GetFrameDataCallback(const char* pName, int index,
      void* pData, Type& frame)
{
  XR_ASSERT(ActorReader, pName != 0);
  XR_ASSERT(ActorReader, pData != 0);
  SessionData*  pSessionData(static_cast<SessionData*>(pData));

  HardString<128> name(pSessionData->pActorName);
  name += "_";
  name += pName;

  return (*pSessionData->pGetFrameDataCb)(name.c_str(), index,
    pSessionData->pGetFrameDataCbData, frame);
}

} // XR

#endif  //XR_ACTORREADER_HPP