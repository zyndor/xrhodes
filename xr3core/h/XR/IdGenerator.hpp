#ifndef XR_IDGENERATOR_HPP
#define XR_IDGENERATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <cstdint>
#include "debug.hpp"

namespace XR
{

//==============================================================================
class IdGenerator
{
public:
  // types
  using IdType = uint32_t;

  // static
  static IdType const kInvalidId;

  // structors
  IdGenerator()
  : m_id(0)
  {}

  void SetNextId(IdType id)
  {
    //XR_ASSERTMSG(IdGenerator, id != kInvalidId, "Cannot set reserved invalid id as next.");
    m_id = id;
  }

  IdType Generate()
  {
    const IdType id = m_id;
    //XR_ASSERT(IdGenerator, id != kInvalidId);
    ++m_id;
    return id;
  }

private:
  IdType m_id;
};

} // XR

#endif