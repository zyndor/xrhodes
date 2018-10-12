#ifndef XR_SERIALIZABLE_HPP
#define XR_SERIALIZABLE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <iostream>

namespace xr
{

class Deflator;
class Inflator;

//==============================================================================
///@brief The Inflatable interface facilitates the writing and the restoring
/// of objects' states, including pointers to other Inflatable members.
class Inflatable
{
public:
  virtual ~Inflatable()
  {}

  ///@brief Writes the state of the object into a stream. A Deflator is
  /// provided to obtain ids for data members that are pointers to other
  /// Inflatable objects that this doesn't own. These ids are then written
  /// instead of the pointers.
  ///@note The higher level implementation needs to make sure that all
  /// Inflatable instances are registered on the Deflator before Serialize() is
  /// being called.
  virtual void Serialize(Deflator const& deflator, std::ostream& stream) =0;

  ///@brief Reads the state of the object from a stream. An Inflator is
  /// provided to register data members that are pointers to other
  /// Inflatable objects that this doesn't own, mapping them to the
  /// ids that are being read in place of them.
  ///@note The higher level implementation needs to make sure that
  /// ResolveReferences() is called at the end of deserialization.
  virtual void Restore(std::istream& stream, Inflator& inflator) =0;

  ///@brief Provides an opportunity for post-processing after the hierarchy
  /// of Inflatable objects are Restore()d and the references are resolved
  /// by the Inflator.
  virtual void  OnInflated()
  {}
};

}

#endif
