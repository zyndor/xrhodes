//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_STREAMUTILS_HPP
#define XR_STREAMUTILS_HPP

#include <iomanip>

//==============================================================================
///@brief Writes @a value as a byte array into the given stream.
template <typename T>
bool  WriteBinaryStream(T const& value, std::ostream& s);

///@brief Reads @a value as a byte array from the given stream.
template <typename T>
bool  ReadBinaryStream(std::istream& s, T& value);

//==============================================================================
// inline
//==============================================================================
template <typename T>
inline
bool  WriteBinaryStream(T const& value, std::ostream& s)
{
  s.write(reinterpret_cast<char const*>(&value), sizeof(T));
  return s.good();
}

//==============================================================================
template <typename T>
inline
bool  ReadBinaryStream(std::istream& s, T& value)
{
  s.read(reinterpret_cast<char*>(&value), sizeof(T));
  return s.good();
}

#endif //XR_STREAMUTILS_HPP