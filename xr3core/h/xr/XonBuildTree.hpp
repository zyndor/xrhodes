#ifndef XR_XONREADER_HPP
#define XR_XONREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "XonParser.hpp"
#include "XonEntity.hpp"
#include <memory>

namespace xr
{

///@brief Attempts to parse @a string as XON and build a tree of XON entities.
/// Adds some semantics-related handling to the XON parser: an unquoted 'null'
/// string is interpreted as a null value.
///@param string - the buffer to parse. Does not have to be 0-terminated.
///@param length - the size of the buffer.
///@param stateOut - optional; if provided, the parser state will be written
/// to it.
///@return A pointer to the root object of the XON document, or nullptr if the
/// parse has failed.
///@note Transfers ownership of the XonObject (and its children).
XonObject* XonBuildTree(char const* string, size_t length,
  XonParser::State* outState = nullptr);

} // xr

#endif  //XR_XONREADER_HPP
