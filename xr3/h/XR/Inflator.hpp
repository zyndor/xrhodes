//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_INFLATOR_HPP
#define XR_INFLATOR_HPP

#include "IdGenerator.hpp"
#include "Inflatable.hpp"
#include "fundamentals.hpp"
#include "debug.hpp"
#include "typeutils.hpp"
#include <map>
#include <set>

namespace XR
{

//==============================================================================
///@brief Facilitates the deserialization of an object hierarchy by storing
/// mappings from IDs to pointer references which can be filled out once
/// all objects are restored. This is a two step process: 1, Object data is
/// read back. For all pointers to serialized objects, an ID is read and the ID
/// to pointer mapping is registered. 2, The mappings are resolved.
///@note Heavy duty multiple inheritance is not guaranteed to play nicely with
/// it, then again, what is heavy duty multiple inheritance guaranteed to
/// play nicely with?
class Inflator
{
  XR_NONCOPY_DECL(Inflator)

public:
  using IdType = IdGenerator::IdType;

  Inflator();

  ///@brief Sets the ID that will be assigned to the next object.
  void SetNext(IdType id);

  ///@brief As an ID is read during deserialization, this method
  /// registers the reference to the pointer that the ID should
  /// be inflated into.
  template <class T>
  void  RegisterMapping(IdType id, T *&p)
  {
    AssertBase<std::decay<T>::type, Inflatable>();
#ifdef XR_DEBUG
    p = nullptr;
#endif
    if (id != IdGenerator::kInvalidId)
    {
      m_mappings[id].insert(&p);
    }
  }

  ///@brief Once an object is restored and its mappings are registered,
  /// this method assigns an ID to the object. Once all objects are
  /// restored (and their mappings are registered), the mappings can be
  /// resolved, which completes the deserialization.
  ///@note A Inflatable can be registered as soon as it's an object
  /// instance with an address, however it's probably better left
  /// until its data is fully read and its mappings are registered.
  IdType RegisterObject(Inflatable& s);

  ///@brief Traverses the database of pointer references mapped to IDs
  /// and resolves them, filling out the pointers.
  void ResolveMappings();

private:
  using IdMap = std::map<IdType, Inflatable*>;
  using RefSet = std::set<void*>;
  using RefMap = std::map<IdType, RefSet>;

  IdGenerator m_generator;
  IdMap       m_objects; // no ownership
  RefMap      m_mappings;

  using ObjectMap = std::map<Inflatable*, IdType>;
  ObjectMap m_ids; // no ownership still
};

}
#endif