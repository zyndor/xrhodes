#include "Serialized.hpp"

namespace XR
{

//==============================================================================
Serialized::IdType        Serialized::s_nextId;
Serialized::IdMap         Serialized::s_ids;
Serialized::JobList       Serialized::s_jobs;
Serialized::SerializedMap Serialized::s_serialized;

//==============================================================================
void  Serialized::SetNextId(IdType id)
{
  XR_ASSERT(Serialized, id != INVALID_ID);
  s_nextId = id;
}

//==============================================================================
void  Serialized::ResetSerialization()
{
  s_nextId = 0;
  s_ids.clear();
}

//==============================================================================
Serialized::IdType  Serialized::RegisterSerialized(const Serialized* pObj)
{
  XR_ASSERT(Serialized, pObj != 0);
  XR_ASSERT(Serialized, s_ids.find(pObj) == s_ids.end());
  IdType id = s_nextId;
  ++s_nextId;
  s_ids[pObj] = id;
  return id;
}

//==============================================================================
Serialized::IdType  Serialized::GetSerializedId(const Serialized* pObj)
{
  IdType id(INVALID_ID);
  if (pObj != 0)
  {
    IdMap::iterator iFind(s_ids.find(pObj));
    if(iFind != s_ids.end())
    {
      id = iFind->second;
    }
  }
  return id;
}

//==============================================================================
bool  Serialized::SerializeId(const Serialized* pObj, int hFile)
{
  XR_ASSERT(Serialized, hFile != File::INVALID_HANDLE);
  IdType  id(GetSerializedId(pObj));
  return File::Write(&id, sizeof(IdType), 1, hFile) == 1;
}

//==============================================================================
void  Serialized::ResetDeserialization()
{
  s_nextId = 0;
  s_jobs.clear();
  s_serialized.clear();
}

//==============================================================================
void  Serialized::RegisterDeserialized(Serialized* pObj)
{
  XR_ASSERT(Serialized, s_serialized.find(s_nextId) == s_serialized.end());
  s_serialized[s_nextId] = pObj;
  ++s_nextId;
}

//==============================================================================
void  Serialized::ResolveReferences()
{
  for (JobList::iterator i0(s_jobs.begin()), i1(s_jobs.end()); i0 != i1; ++i0)
  {
    SerializedMap::iterator iFind(s_serialized.find(i0->id));
      
    Serialized* pObject(iFind != s_serialized.end() ? iFind->second : 0);
    *static_cast<Serialized**>(i0->ppInflatable) = pObject;

    if (i0->pInflateCb != 0)
    {
      (*i0->pInflateCb)(pObject, i0->pInflateCbData);
    }
  }

  s_jobs.clear();
}

} //XR