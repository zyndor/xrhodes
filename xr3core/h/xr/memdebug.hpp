#ifndef XR_MEMDEBUG_HPP
#define XR_MEMDEBUG_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <string>

namespace xr
{

//==============================================================================
///@brief Writes the contents of @a numBytes bytes of memory starting at
/// @a buffer, into @a os, in a format similar to Visual Studio's Memory Window:
/// {address} {two-digit hexadecimal values of 16 bytes} {visualisation of the
/// same 16 bytes (non-printable characters are replaced with '.'-s.)}
void LogMemory(void const* buffer, size_t numBytes, std::ostream& os);

///@brief Writes the contents of @a numBytes bytes of memory starting at
/// @a buffer, into the string that it returns.
std::string LogMemory(void const* buffer, size_t numBytes);

//==============================================================================
///@brief Compares the first @a numBytes bytes of the buffers starting at
/// @a buffer0 and @a buffer1 and writes the consecutive sections of different
/// bytes into @a os, in the following format:
/// +{offset of difference}:{number of consecutive bytes different}
/// {two-digit hexadecimel values of bytes from first buffer} |
/// {two-digit hexadecimel values of bytes from second buffer}.
void DiffMemory(void const* buffer0, void const* buffer1, size_t numBytes, std::ostream& os);

///@brief Compares the first @a numBytes bytes of the buffers starting at
/// @a buffer0 and @a buffer1 and writes the results into the string that it
/// returns.
std::string DiffMemory(void const* buffer0, void const* buffer1, size_t numBytes);

} // xr

#endif //XR_MEMDEBUG_HPP
