//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_STRINGUTILS_HPP
#define XR_STRINGUTILS_HPP

#include <cstdint>

namespace XR
{

///@brief Finds all instances of @a find in @a original and replaces them with
/// @replace, writing no more than @a bufferSize characters of the result
/// to @a buffer and the actual length of processed string to @a replacedSize.
/// Does not allocate memory.
///@note Will not null terminate @a buffer.
///@param original: original string. May contain \0 characters; is not required
/// to be null-terminated.
///@param originalSize: size of original string.
///@param find: null-terminated string to find.
///@param replace: null-terminated string to replace instances of find.
///@param bufferSize: the size of the provided buffer. No more than this many
/// characters will be written.
///@param buffer: buffer to write result to.
///@param replacedSize: number of characters actually written.
char const* Replace(char const* original, size_t originalSize, char const* find,
  char const* replace, size_t bufferSize, char* buffer, size_t& replacedSize);

///@brief This overload of Replace() doesn't require the size of the original
/// string provided, which means it needs to be null-terminated.
char const* Replace(char const* original, char const* find, char const* replace,
  size_t bufferSize, char* buffer, size_t& replacedSize);

}

#endif //XR_STRINGUTILS_HPP