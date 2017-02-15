//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_STREAMUTILS_HPP
#define XR_STREAMUTILS_HPP

#include <iomanip>

namespace XR
{

//==============================================================================
///@brief Writes @a value into the given stream as a byte array. If specified,
/// @a pOutNumBytesWritten is incremented by the size of T upon successful
/// insertion into the stream.
///@return Whether the operation was successful, i.e. !@a s.fail().
template <typename T>
bool  WriteBinaryStream(T const& value, std::ostream& s, size_t* pOutNumBytesWritten = nullptr);

///@brief Reads @a value from the stream @a s as a byte array. If specified,
/// @a pOutNumBytesRead is incremented by the size of T upon successful
/// extraction from the stream.
///@return Whether the operation was successful, i.e. !@a s.fail().
template <typename T>
bool  ReadBinaryStream(std::istream& s, T& value, size_t* pOutNumBytesRead = nullptr);

///@brief Writes the number of elements (as SizeType), then the elements
/// themselves in the [@a iBegin, @a iEnd) range to the stream @a s, as a
/// byte array. If specified, @a pOutNumBytesWritten is incremented by the
/// size of T times the number of elements successfully inserted into the
/// stream. Note that this doesn't include the bytes written as part of 
/// the number of elements.
///@return Whether the operation was successful, i.e. !@a s.fail().
template <typename SizeType = uint32_t, typename Iter>
bool  WriteRangeBinaryStream(Iter iBegin, Iter iEnd, std::ostream& s,
  size_t* pOutNumBytesWritten = nullptr);

///@brief Reads the number of elements (as SizeType), then the elements
/// themselves from the stream @a s, as a byte array, assigning them to
/// the output iterator @a iOut. If specified, @a pOutNumBytesRead is
/// incremented by the size of T times the number of elements successfully
/// extracted from the stream. Note that this doesn't include the bytes read as
/// part of the number of elements.
///@return Whether the operation was successful, i.e. !@a s.fail().
template <typename SizeType = uint32_t, typename Iter>
bool  ReadRangeBinaryStream(std::istream& s, Iter iOut,
  size_t* pOutNumBytesRead = nullptr);

//==============================================================================
// inline
//==============================================================================
template <typename T>
inline
bool  WriteBinaryStream(T const& value, std::ostream& s, size_t* pOutNumBytesWritten)
{
  s.write(reinterpret_cast<char const*>(&value), sizeof(T));
  const bool success = bool(s);
  if (success && pOutNumBytesWritten)
  {
    *pOutNumBytesWritten += sizeof(T);
  }
  return success;
}

//==============================================================================
template <typename T>
inline
bool  ReadBinaryStream(std::istream& s, T& value, size_t* pOutNumBytesRead)
{
  s.read(reinterpret_cast<char*>(&value), sizeof(T));
  const bool success = bool(s);
  if (success && pOutNumBytesRead)
  {
    *pOutNumBytesRead += sizeof(T);
  }
  return success;
}

//==============================================================================
template <typename SizeType, typename Iter>
inline
bool  WriteRangeBinaryStream(Iter iBegin, Iter iEnd, std::ostream& s, size_t* pOutNumBytesWritten)
{
  SizeType numElems = std::distance(iBegin, iEnd);
  bool result = WriteBinaryStream(numElems, s, nullptr);
  while (result && iBegin != iEnd)
  {
    result = WriteBinaryStream(*iBegin, s, pOutNumBytesWritten);
    ++iBegin;
  }
  return result;
}

//==============================================================================
template <typename SizeType, typename Iter>
inline
bool  ReadRangeBinaryStream(std::istream& s, Iter iOut, size_t* pOutNumBytesRead)
{
  SizeType numElems;
  bool result = ReadBinaryStream(s, numElems, nullptr);
  while (result && numElems > 0)
  {
    result = ReadBinaryStream(s, *iOut, pOutNumBytesRead);
    --numElems;
  }
  return result;
}

} // XR

#endif //XR_STREAMUTILS_HPP