//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  <string.h>
#include  "Named.hpp"

namespace XR
{

//==============================================================================
Named::Named ()
: m_name(0)
{}

//==============================================================================
Named::Named(uint32 name)
: m_name(name)
{}

//==============================================================================
Named::Named(const char* pName)
: m_name(TransformName(pName, strlen(pName)))
{}

//==============================================================================
Named::~Named ()
{}

//==============================================================================
void  Named::SetName(uint32 name)
{
  m_name = name;
}

//==============================================================================
void  Named::SetName(const char* pName)
{
  XR_ASSERTMSG(Named, pName != 0, ("Null name is invalid."));
  XR_ASSERTMSG(Named, strlen(pName) > 0, ("Empty name is invalid."));
  m_name = TransformName(pName, strlen(pName));
}

} // XR
