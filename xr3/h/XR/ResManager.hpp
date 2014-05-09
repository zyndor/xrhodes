//
// Nuclear Heart Games
// XRhodes
//
// ResManager.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    10/14/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_RESMANAGER_HPP
#define XR_RESMANAGER_HPP

#include "types.hpp"
#include "Pool.hpp"
#include "Hash.hpp"
#include "Dictionary.hpp"

namespace XR
{

//==============================================================================
template  <class T>
class ResManager
{
public:
  // static
  static void   Init(int capacityBytes);
  static void   Exit();

  static T*     Get(uint32 hash);
  static T*     Get(const char* pName);
  static T*     Get(const char* pName, void* pData);

  static void   PushFrame();
  static void   PopFrame();
  static void   FlushFrame();

  // intended for use by T only
  static bool   Manage(uint32 hash, T* pObj);
  static bool   Manage(const char* pName, T* pObj);

  static void*  Allocate(int numBytes);
  
private:
  // types
  typedef Dictionary<uint32, T*>  ObjectMap;

  // data
  static Pool*      s_pPool;
  static ObjectMap  s_objects;

  // internal
  static void  _DestructFrameObjects();
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
Pool* ResManager<T>::s_pPool(0);

//==============================================================================
template  <class T>
typename ResManager<T>::ObjectMap ResManager<T>::s_objects;

//==============================================================================
template  <class T>
void ResManager<T>::Init( int capacityBytes )
{
  XR_ASSERT(ResManager, s_pPool == 0);
  XR_ASSERT(ResManager, capacityBytes > 0);

  s_pPool = new Pool(capacityBytes);
}

//==============================================================================
template  <class T>
void ResManager<T>::Exit()
{
  XR_ASSERT(ResManager, s_pPool != 0);

  for(typename ObjectMap::iterator i0(s_objects.begin()), i1(s_objects.end());
    i0 != i1; ++i0)
  {
    i0->value->ManagedDestruct();
    i0->value->~T();
  }
  
  s_objects.clear();

  delete s_pPool;
}

//==============================================================================
template  <class T>
bool  ResManager<T>::Manage( uint32 hash, T* pObj)
{
  typename ObjectMap::iterator iFind(s_objects.find(hash));
  bool  success(iFind == s_objects.end());
  if(success)
  {
    s_objects[hash] = pObj;
  }
  return success;
}

//==============================================================================
template  <class T>
inline
bool  ResManager<T>::Manage( const char* pName, T* pObj)
{
  return Manage(Hash::String(pName));
}

//==============================================================================
template  <class T>
T* ResManager<T>::Get( uint32 hash )
{
  typename ObjectMap::iterator iFind(s_objects.find(hash));
  T*  pObj(0);
  if(iFind != s_objects.end())
  {
    pObj = iFind->value;
  }
  return pObj;
}

//==============================================================================
template  <class T>
inline
T* ResManager<T>::Get( const char* pName )
{
  return Get(Hash::String(pName));
}

//==============================================================================
template  <class T>
inline
T* ResManager<T>::Get( const char* pName, void* pData )
{
  T*  pObj(Get(pName));
  return pObj != 0 ? pObj : static_cast<T*>(pData);
}

//==============================================================================
template  <class T>
void ResManager<T>::PushFrame()
{
  XR_ASSERT(ResManager, s_pPool != 0);
  s_pPool->Push();
}  

//==============================================================================
template  <class T>
void ResManager<T>::PopFrame()
{
  XR_ASSERT(ResManager, s_pPool != 0);
  XR_ASSERT(ResManager, s_pPool->GetNumFrames() > 0);
  s_pPool->Pop();
  _DestructFrameObjects();

}

//==============================================================================
template  <class T>
void ResManager<T>::FlushFrame()
{
  XR_ASSERT(ResManager, s_pPool != 0);
  XR_ASSERT(ResManager, s_pPool->GetNumFrames() > 0);
  s_pPool->Flush();
  _DestructFrameObjects();
}

//==============================================================================
template  <class T>
void ResManager<T>::_DestructFrameObjects() 
{
  void* pCurrent(s_pPool->Allocate(0));
  for(typename ObjectMap::iterator i0(s_objects.begin()), i1(s_objects.end()); i0 != i1; ++i0)
  {
    if(i0->value >= pCurrent)
    {
      i0->value->ManagedDestruct();
      i0->value->~T();
      i0->value = 0;
    }
  }
}

//==============================================================================
template  <class T>
void* ResManager<T>::Allocate( int numBytes )
{
  return s_pPool->Allocate(numBytes);
}

} // XR

//==============================================================================
#define XR_MANAGED_DECL(a)  \
public:\
  typedef ResManager<a> Manager;\
  typedef a*(*GetCallback)(const char* pName, void* pData);\
  \
  static a* CreateManaged(const char* pName) {\
              XR_ASSERT(a, pName != 0);\
              return CreateManaged(Hash::String(pName));\
            }\
  static a* CreateManaged(uint32 hash);\
  \
  void ManagedDestruct();\

//==============================================================================
#define XR_MANAGED_DEF(a) \
a*  a::CreateManaged(uint32 hash) {\
  a*  pRes(Manager::Get(hash));\
  if(pRes == 0)\
  {\
    void* pMem(Manager::Allocate(sizeof(a)));\
    if(pMem != 0)\
    {\
      pRes = new (pMem) a;\
    }\
  }\
  return pRes;\
}\
\
void  a::ManagedDestruct()\
{\
  this->~a();\
}\

//==============================================================================
#define XR_CROSS_MANAGED_DEF(a, aImpl)  \
a*  a::CreateManaged(uint32 hash) {\
  a*  pRes(Manager::Get(hash));\
  if(pRes == 0) {\
    void* pMem(Manager::Allocate(sizeof(aImpl) + sizeof(a)));\
    if(pMem != 0) {\
      aImpl*  pResImpl(new (pMem) aImpl);\
      pRes = new (pResImpl + 1) a;\
      pRes->SwapImpl(pResImpl);\
    }\
  }\
  return pRes;\
}\
\
void  a::ManagedDestruct()  {\
  XR_ASSERT(a, HasImpl());\
  static_cast<aImpl*>(SwapImpl(0))->~aImpl();\
}\

#endif // XR_RESMANAGER_HPP