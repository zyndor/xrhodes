#include <ctype.h>
#include <string.h>
#include <map>
#include <string>
#include "Hash.hpp"

namespace XR
{

//==============================================================================
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
typedef std::map<uint32, std::string> StringMap;

static StringMap	s_stringLookup;
#endif	//XR_DEBUG

//==============================================================================
uint32	Hash::s_seed(Hash::kSeed);

//==============================================================================
void	Hash::SetSeed(uint32 seed)
{
	s_seed = seed;
}

//==============================================================================
uint32	Hash::String(const char* pString)
{
	XR_ASSERT(Hash, pString != 0);
	uint32 hash = s_seed;
	int c;
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
	const char* pSaved(pString);
#endif	//XR_DEBUG
	while ((c = *pString) != 0)
	{
		hash += (hash << 5) + (hash + tolower(c));
		++pString;
	}

#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
	StringMap::iterator iFind(s_stringLookup.find(hash));
	XR_ASSERTMSG(Hash, iFind == s_stringLookup.end() || strcasecmp(pSaved,
		iFind->second.c_str()) == 0, ("Hash collision - %s vs %s (%d)", pSaved,
			iFind->second.c_str(), hash));
	if (iFind == s_stringLookup.end())
	{
		s_stringLookup[hash] = pSaved;
	}
#endif	//XR_DEBUG

	return hash;
}

//==============================================================================
uint32	Hash::String(const char* pString, size_t size)
{
	XR_ASSERT(Hash, pString != 0);
	uint32 hash = s_seed;
	int c;
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
	const char* pSaved(pString);
	int savedSize(size);
#endif	//XR_DEBUG
	while (size > 0 && (c = *pString) != 0)
	{
		hash += (hash << 5) + (hash + tolower(c));
		++pString;
		--size;
	}

#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
	StringMap::iterator iFind(s_stringLookup.find(hash));
	// look for index of first non matching character being @a size
	XR_ASSERTMSG(Hash, iFind == s_stringLookup.end() || strncasecmp(pSaved,
		iFind->second.c_str(), savedSize) == 0, ("Hash collision - %s vs %s (%d)",
			pSaved, iFind->second.c_str(), hash));
	if (iFind == s_stringLookup.end())
	{
		s_stringLookup[hash].assign(pSaved, savedSize);
	}
#endif	//XR_DEBUG

	return hash;
}

//==============================================================================
uint32	Hash::Data(const void* pData, size_t size)
{
	XR_ASSERT(Hash, size >= 0);
	uint32 hash = s_seed;
	const char* p0(static_cast<const char*>(pData));
	const char* p1(p0 + size);
	while (p0 != p1)
	{
		hash += (hash << 5) + (hash + *p0);
		++p0;
	}
	return hash;
}

//==============================================================================
void	Hash::DebugClearStringLookup()
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
	s_stringLookup.clear();
#endif
}

} // XR