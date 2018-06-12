#ifndef XR_DEFLATOR_HPP
#define XR_DEFLATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Inflatable.hpp"
#include "xr/IdGenerator.hpp"
#include "xr/fundamentals.hpp"
#include <map>
#include <set>

namespace xr
{

//==============================================================================
///@brief Facilitates the conversion of pointers to Inflatable objects into
/// IDs that can be serialized. This is a two step process: 1, the Inflatables
/// need to be registered on the Deflator, so that IDs are created, then 2,
/// upon serializing the objects [that may have references to these registered
/// objects], the client code gets and writes these IDs instead.
///@note Heavy duty multiple inheritance is not guaranteed to play nicely with
/// it, then again, what is heavy duty multiple inheritance guaranteed to
/// play nicely with?
///@note There's a vague assumption about the Inflatable objects in the
/// hierarchy not owning each other. It can be done if they are, but you might
/// need multiple passes.
class Deflator
{
  XR_NONCOPY_DECL(Deflator)

public:
  using IdType = IdGenerator::IdType;

  Deflator();

  ///@brief Sets the ID that will be assigned to the next object.
  void SetNext(IdType id);

  ///@brief Registers an object that will be serialized, and will have
  /// pointers to it serialized.
  IdType RegisterObject(Inflatable const& object);

  ///@brief Retrieves the ID associated with a Inflatable.
  ///@note As there's no way to register a nullptr (null reference), this will
  /// return kInvalidId for a nullptr, which is what we want.
  IdType GetId(Inflatable const* object) const;

private:
  IdGenerator m_generator;

  using IdMap = std::map<Inflatable const*, IdType>;
  IdMap m_objects;  // no ownership

  using IdSet = std::set<IdType>;
  IdSet m_ids;
};

}

#endif //XR_DEFLATOR_HPP
