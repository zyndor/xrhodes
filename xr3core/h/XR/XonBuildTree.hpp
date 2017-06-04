//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_XONREADER_HPP
#define XR_XONREADER_HPP

#include "XonParser.hpp"
#include "XonEntity.hpp"
#include <memory>

namespace XR
{

///@brief Attempts to parse @a pBuffer as XON and build a tree of XON entities.
/// Adds some semantics-related handling to the XON parser: an unquoted 'null'
/// string is interpreted as a null value.
///@param pBuffer - the buffer to parse. Does not have to be 0-terminated.
///@param length - the size of the buffer.
///@param pStateOut - optional; if provided, the parser state will be written
/// to it.
///@return The root object of the XON document.
XonObject* XonBuildTree(char const* pBuffer, size_t length,
  XonParser::State* pStateOut = nullptr);

} // XR

#endif  //XR_XONREADER_HPP
