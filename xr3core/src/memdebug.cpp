//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/memdebug.hpp"
#include <sstream>
#include <iomanip>
#include <stdint.h>

namespace xr
{

//==============================================================================
std::string LogMemory(void const* buffer, size_t numBytes)
{
  std::ostringstream stream;
  LogMemory(buffer, numBytes, stream);
  return stream.str();
}

void LogMemory(void const* buffer, size_t numBytes, std::ostream & os)
{
  uint8_t const* readp = static_cast<uint8_t const*>(buffer);

  uint8_t const* readEnd = readp + numBytes;
  while (readp < readEnd)
  {
    os << static_cast<const void*>(readp) << "  " << std::hex <<
      std::setw(2U) << std::setfill('0') << int(*readp);
    ++readp;

    std::ostringstream visBuffer;
    for (size_t i = 0; i < 15; ++i)
    {
      os << " " << std::setw(2U) << std::setfill('0') << int(*readp);
      visBuffer << ((*readp >= ' ') ? char(*readp) : '.');
      ++readp;
    }

    os << "  " << visBuffer.str() << "\n";
  }
}

//==============================================================================
std::string DiffMemory(void const* buffer0, void const* buffer1, size_t numBytes)
{
  std::ostringstream stream;
  DiffMemory(buffer0, buffer1, numBytes, stream);
  return stream.str();
}

void DiffMemory(void const* buffer0, void const* buffer1, size_t numBytes, std::ostream& os)
{
  uint8_t const* readp0 = static_cast<uint8_t const*>(buffer0);
  uint8_t const* readp1 = static_cast<uint8_t const*>(buffer1);
  size_t offset = 0;

  while (offset < numBytes)
  {
    if (readp0[offset] != readp1[offset])
    {
      size_t startOffset = offset;

      std::ostringstream memBuffer0;
      std::ostringstream memBuffer1;

      memBuffer0 << std::hex << std::setw(2U) << std::setfill('0') << int(readp0[offset]);
      memBuffer1 << std::hex << std::setw(2U) << std::setfill('0') << int(readp1[offset]);
      size_t diffBytes = 1;

      while (readp0[offset] != readp1[offset] && offset < numBytes)
      {
        memBuffer0 << " " << std::setw(2U) << std::setfill('0') << int(readp0[offset]);
        memBuffer1 << " " << std::setw(2U) << std::setfill('0') << int(readp1[offset]);
        ++offset;
        ++diffBytes;
      }

      // format: +{hex offset of difference}:{number of bytes different} {buffer 0} | {buffer 1}
      os << "+" << std::hex << std::setw(8) << std::setfill('0') << startOffset <<
        ":" << std::setw(8) << std::setfill('0') << diffBytes << "  " << memBuffer0.str() << " | " << memBuffer1.str() << "\n";
    }
    ++offset;
  }

}

} // xr
