#ifndef XR_STRINGUTILS_HPP
#define XR_STRINGUTILS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <cstdint>
#include <sstream>

namespace xr
{

///@return @a original, or if it's null, an empty string.
char const* GetStringSafe(char const* original);

///@brief Attempts to convert @a from to @a to.
///@return Whether the operation was successful.
template  <typename T>
bool  StringTo(char const* from, T& to);

///@brief Finds all instances of @a find in @a original and replaces them with
/// @replace, writing no more than @a bufferSize characters of the result to
/// @a buffer and the actual length of the processed string to @a processedSize.
/// Does not allocate memory. If, after writing the string there's space left in
/// the buffer (i.e. processedSize < bufferSize), it will write a null terminator.
///@param original: original string. May contain \0 characters; is not required
/// to be null-terminated.
///@param originalSize: size of original string.
///@param find: null-terminated string to find.
///@param replace: null-terminated string to replace instances of find.
///@param bufferSize: the size of the provided buffer. No more than this many
/// characters will be written.
///@param buffer: buffer to write the result to.
///@param processedSize: output. The number of characters actually written.
/// Guaranteed to be less then or equal to @a bufferSize.
///@return The start of the replaced string.
char const* Replace(char const* original, size_t originalSize, char const* find,
  char const* replace, size_t bufferSize, char* buffer, size_t& processedSize);

///@brief This overload of Replace() operates on a null-terminated string,
/// not requiring the size of @a original to be supplied.
char const* Replace(char const* original, char const* find, char const* replace,
  size_t bufferSize, char* buffer, size_t& processedSize);

///@brief Converts the character @a c into its two-byte textual hex
/// representation.
///@return One past the last character written to in @a buffer.
///@note Does not null terminate.
char* Char2Hex(char c, char buffer[2]);

///@brief URL-encodes the first @a originalSize characters of @a original,
/// writing up to @a bufferSize characters into the provided @a buffer and the
/// actual length of the processed string to @a processedSize.
/// Writes a null terminator if it can.
///@param original: original string. May contain \0 characters; is not required
/// to be null-terminated.
///@param originalSize: size of original string.
///@param bufferSize: the size of the provided buffer. No more than this many
/// characters will be written.
///@param buffer: buffer to write the result to.
///@param processedSize: output. The number of characters actually written.
/// Guaranteed to be less then or equal to @a bufferSize.
///@return One past the last character written to in @a buffer, NOT including
/// a null terminator it may have written.
char const* UrlEncode(char const* original, size_t originalSize, size_t bufferSize,
  char* buffer, size_t& processedSize);

///@brief This overload of UrlEncode() operates on a null-terminated string,
/// not requiring the size of @a original to be supplied.
char const* UrlEncode(char const* original, size_t bufferSize, char* buffer,
  size_t& processedSize);

//==============================================================================
// inline
//==============================================================================
inline
char const* GetStringSafe(char const* original)
{
  return original != nullptr ? original : "";
}

//==============================================================================
template  <typename T>
inline
bool  StringTo(const char* from, T& to)
{
  std::istringstream  iss(from);

  return !(iss >> std::ws >> to).fail() && (iss >> std::ws).eof();
}

} // xr

#endif //XR_STRINGUTILS_HPP
