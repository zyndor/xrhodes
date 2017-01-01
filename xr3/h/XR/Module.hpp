//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MODULE_HPP
#define XR_MODULE_HPP

#include "types.hpp"

namespace XR {

//==============================================================================
class Module
{
  XR_NONCOPY_DECL(Module)

public:
  // structors
  Module();
  ~Module();
  
  // virtual
  virtual void  Update(int32 tDelta) =0;
  virtual void  Render() =0;
};

} // XR

#endif // XR_MODULE_HPP
