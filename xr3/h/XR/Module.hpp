//
// Nuclear Heart Games
// XRhodes
//
// Module.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    24/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_MODULE_HPP
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
