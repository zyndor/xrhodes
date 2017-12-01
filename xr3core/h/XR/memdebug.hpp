#ifndef XR_MEMDEBUG_HPP
#define XR_MEMDEBUG_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <string>

namespace XR
{

//==============================================================================
///@brief Writes the contents of @a numBytes bytes of memory starting at
/// @a pMem, into @a os, in a format similar to Visual Studio's Memory Window:
/// {address} {two-digit hexadecimal values of 16 bytes} {visualisation of the
/// same 16 bytes (non-printable characters are replaced with '.'-s.)}
void LogMemory(void const* pMem, size_t numBytes, std::ostream& os);

///@brief Writes the contents of @a numBytes bytes of memory starting at
/// @a pMem, into the string that it returns.
std::string LogMemory(void const* pMem, size_t numBytes);

//==============================================================================
///@brief Compares the first @a numBytes bytes of the buffers starting at
/// @a pMem0 and @a pMem1 and writes the consecutive sections of different
/// bytes into @a os, in the following format:
/// +{offset of difference}:{number of consecutive bytes different}
/// {two-digit hexadecimel values of bytes from first buffer} | 
/// {two-digit hexadecimel values of bytes from second buffer}.
void DiffMemory(void const* pMem0, void const* pMem1, size_t numBytes, std::ostream& os);

///@brief Compares the first @a numBytes bytes of the buffers starting at
/// @a pMem0 and @a pMem1 and writes the results into the string that it
/// returns.
std::string DiffMemory(void const* pMem0, void const* pMem1, size_t numBytes);

} // XR

#endif //XR_MEMDEBUG_HPP