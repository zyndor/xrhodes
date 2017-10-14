#include "XR/memdebug.hpp"
#include <sstream>
#include <iomanip>
#include <stdint.h>

namespace XR
{

//==============================================================================
std::string LogMemory(void const * pMem, size_t numBytes)
{
  std::ostringstream buffer;
  LogMemory(pMem, numBytes, buffer);
  return buffer.str();
}

void LogMemory(void const * pMem, size_t numBytes, std::ostream & os)
{
  uint8_t const* pRead = static_cast<uint8_t const*>(pMem);

  uint8_t const* pEnd = pRead + numBytes;
  while (pRead < pEnd)
  {
    os << static_cast<const void*>(pRead) << "  " << std::hex <<
      std::setw(2U) << std::setfill('0') << int(*pRead);
    ++pRead;

    std::ostringstream visBuffer;
    for (size_t i = 0; i < 15; ++i)
    {
      os << " " << std::setw(2U) << std::setfill('0') << int(*pRead);
      visBuffer << ((*pRead >= ' ') ? char(*pRead) : '.');
      ++pRead;
    }

    os << "  " << visBuffer.str() << "\n";
  }
}

//==============================================================================
std::string DiffMemory(void const * pMem0, void const * pMem1, size_t numBytes)
{
  std::ostringstream buffer;
  DiffMemory(pMem0, pMem1, numBytes, buffer);
  return buffer.str();
}

void DiffMemory(void const* pMem0, void const* pMem1, size_t numBytes, std::ostream& os)
{
  uint8_t const* pRead0 = static_cast<uint8_t const*>(pMem0);
  uint8_t const* pRead1 = static_cast<uint8_t const*>(pMem1);
  size_t offset = 0;

  while (offset < numBytes)
  {
    if (pRead0[offset] != pRead1[offset])
    {
      size_t startOffset = offset;

      std::ostringstream memBuffer0;
      std::ostringstream memBuffer1;

      memBuffer0 << std::hex << std::setw(2U) << std::setfill('0') << int(pRead0[offset]);
      memBuffer1 << std::hex << std::setw(2U) << std::setfill('0') << int(pRead1[offset]);
      size_t diffBytes = 1;

      while (pRead0[offset] != pRead1[offset] && offset < numBytes)
      {
        memBuffer0 << " " << std::setw(2U) << std::setfill('0') << int(pRead0[offset]);
        memBuffer1 << " " << std::setw(2U) << std::setfill('0') << int(pRead1[offset]);
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

} // XR
