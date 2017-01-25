//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DEFLATOR_HPP
#define XR_DEFLATOR_HPP

#include "IdGenerator.hpp"
#include "Serializable.hpp"
#include <map>

namespace XR
{

//==============================================================================
///@brief Facilitates the conversion of pointers to Serializable objects into
/// IDs that can be serialized. This is a two step process: 1, the Serializables
/// need to be registered on the Deflator, so that IDs are created, then 2,
/// upon serializing the objects [that may have references to these registered
/// objects], the client code gets and writes these IDs instead.
///@note Heavy duty multiple inheritance is not guaranteed to play nicely with
/// it, then again, what is heavy duty multiple inheritance guaranteed to
/// play nicely with?
///@note There's a vague assumption about the Serializable objects in the
/// hierarchy not owning each other. It can be done if they are, but you might
/// need multiple passes.
class Deflator
{
public:
  using IdType = IdGenerator::IdType;

  ///@brief Sets the ID that will be assigned to the next object.
  void SetNext(IdType id);

  ///@brief Registers an object that will be serialized, and will have
  /// pointers to it serialized.
  IdType RegisterObject(Serializable const& s);

  ///@brief Retrieves the ID associated with a Serializable.
  ///@note As there's no way to register a nullptr (null reference), this will
  /// return kInvalidId for a nullptr, which is what we want.
  IdType GetId(Serializable const* p) const;

private:
  using IdMap = std::map<Serializable const*, IdType>;

  IdGenerator m_generator;
  IdMap       m_objects;  // no ownership
};

}

#endif //XR_DEFLATOR_HPP