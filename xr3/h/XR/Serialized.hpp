//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SERIALIZED_HPP
#define XR_SERIALIZED_HPP

#include <list>
#include <map>
#include "utils.hpp"
#include "File.hpp"

namespace XR
{

//==============================================================================
class Serialized
{
public:
  // types
  typedef uint32  IdType;

  typedef void(*OnInflateCallback)(Serialized* pObject, void* pUserData);

  // static
  static const IdType kInvalidId = -1;

  ///@brief Sets the id that the next object will be given when
  /// RegisterSerialized() / RegisterDeserialized() is called.
  static void     SetNextId(IdType id);

  ///@brief Resets the next id to 0 and clears the registry of
  /// serialized objects.
  static void     ResetSerialization();

  ///@brief Registers the object @a pObj.
  ///@return  The id assigned to @a pObj.
  static IdType   RegisterSerialized(const Serialized* pObj);

  ///@return  The id assigned to @a pObj, or INVALID_ID, if @a pObj was not
  /// registered.
  static IdType   GetSerializedId(const Serialized* pObj);

  ///@brief Convenience method to get the id of @a pObj and write that to
  /// @a pFile.
  ///@return Whether the operation was successful.
  static bool     SerializeId(const Serialized* pObj, int hFile);

  ///@brief Resets the next id to 0 and clears the list of jobs and the
  /// registry of deserialized items.
  static void     ResetDeserialization();

  ///@brief Registers a reference to a pointer that will be filled out with the
  /// object that @a id is referring to, when ResolveReferences() is called.
  /// @a pInflateCb is called with the pointer to the object and
  /// @a pInflateCbData after this had happened, but are optional.
  template <class T>
  static void     RegisterReference(IdType id, T*& pObject,
    OnInflateCallback pInflateCb, void* pInflateCbData);

  ///@brief Registers a reference to a const pointer that will be filled out
  /// with the object that @a id is referring to, when ResolveReferences() is
  /// called. @a pInflateCb is called with the pointer to the object and
  /// @a pInflateCbData after this had happened, but are optional.
  template <class T>
  static void     RegisterReference(IdType id, const T*& pObject,
    OnInflateCallback pInflateCb, void* pInflateCbData);

  ///@brief Convenience method to read an id from a @a pFile then call
  /// RegisterReference with the result, @a pObject, @a pOnInflateCb and
  /// @pOnInflateCbData.
  ///@return Whether the operation was successful.
  template <class T>
  static bool     DeserializeId(T*& pObject, int hFile,
    OnInflateCallback pOnInflateCb, void* pOnInflateCbData);

  ///@brief Convenience method to read an id from a @a pFile then call
  /// RegisterReference with the result, @a pObject, @a pOnInflateCb and
  /// @pOnInflateCbData.
  ///@return Whether the operation was successful.
  template <class T>
  static bool     DeserializeId(const T*& pObject, int hFile,
    OnInflateCallback pOnInflateCb, void* pOnInflateCbData);

  ///@brief Registers your deserialized object, assigning the next id to it.
  /// This needs to be called after your object has completed deserialisation
  /// (not including the resolution of references of course).
  static void     RegisterDeserialized(Serialized* pObj);

  ///@brief Traverses the list of references, and writes the pointers to the
  /// object of the right id to them, then calls the OnInflateCallbacks
  /// registered for the job, if any, with the pointer to the Serialized object
  /// and user defined callback data, if any.
  static void     ResolveReferences();

  // structors
  Serialized()
  {}

  ~Serialized()
  {}

private:
  // types
  typedef std::map<const Serialized*, IdType> IdMap;

  struct  Job
  {
    IdType            id;
    void*             ppInflatable;
    OnInflateCallback pInflateCb;
    void*             pInflateCbData;
  };

  typedef std::list<Job>                JobList;
  typedef std::map<IdType, Serialized*> SerializedMap;

  // static
  static IdType         s_nextId;
  static IdMap          s_ids;
  static JobList        s_jobs;
  static SerializedMap  s_serialized;
};

//==============================================================================
// implementation
//==============================================================================
template <class T>
bool  Serialized::DeserializeId(T*& pObject, int hFile,
  OnInflateCallback pOnInflateCb, void* pOnInflateCbData)
{
  XR_ASSERT(Serialized, hFile != File::INVALID_HANDLE);
  IdType  id;
  bool    success = File::Read(sizeof(IdType), 1, hFile, &id) == 1;
  if (success)
  {
    RegisterReference(id, pObject, pOnInflateCb, pOnInflateCbData);
  }
  return success;
}

//==============================================================================
template <class T>
bool  Serialized::DeserializeId(const T*& pObject, int hFile,
  OnInflateCallback pOnInflateCb, void* pOnInflateCbData)
{
  XR_ASSERT(Serialized, hFile != File::INVALID_HANDLE);
  IdType  id;
  bool    success(File::Read(sizeof(IdType), 1, hFile, &id) == 1);
  if (success)
  {
    RegisterReference(id, pObject, pOnInflateCb, pOnInflateCbData);
  }
  return success;
}

//==============================================================================
template <class T>
void  Serialized::RegisterReference(IdType id, T*& pObject,
  OnInflateCallback pInflateCb, void* pInflateCbData)
{
  Serialized* pBase(pObject); // type check
  Job j = { id, &pObject, pInflateCb, pInflateCbData };
  s_jobs.push_back(j);
}

//==============================================================================
template <class T>
void  Serialized::RegisterReference(IdType id, const T*& pObject,
  OnInflateCallback pInflateCb, void* pInflateCbData)
{
  const Serialized* pBase(pObject); // type check
  Job j = { id, &pObject, pInflateCb, pInflateCbData };
  s_jobs.push_back(j);
}

} // XR

#endif  //XR_SERIALIZED_HPP