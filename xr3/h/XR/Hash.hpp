//
// Nuclear Heart Games
// XRhodes
//
// Hash.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    10/08/2013
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_HASH_HPP
#define XR_HASH_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
class Hash
{
	XR_NONOBJECT_DECL(Hash)

public:
	// static
	static const uint32 kSeed = 61681;

	static void		SetSeed(uint32 seed);
	
	static uint32	String(const char* pString);
	static uint32	String(const char* pString, size_t size);
	static uint32	Data(const void* pData, size_t size);

	static void		DebugClearStringLookup();

private:
	static uint32	s_seed;
};

} // XR

#endif // XR_HASH_HPP