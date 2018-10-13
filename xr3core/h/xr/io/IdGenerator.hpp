#ifndef XR_IDGENERATOR_HPP
#define XR_IDGENERATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <cstdint>

namespace xr
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

} // xr

#endif
