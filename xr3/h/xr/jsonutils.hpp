#ifndef XR_JSONUTILS_HPP
#define XR_JSONUTILS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "xr/JsonEntity.hpp"

namespace xr
{

//==============================================================================
///@brief Attempts to load a json file under the given path, and process up
/// to @a maxDepth levels. @a quietErrors defines whether errors encountered
/// during processing should be flagged.
///@return Pointer to and ownership of the root element of the document. If
/// an error has occurred, this will be nullptr.
[[nodiscard]] JSON::Entity* LoadJSON(const char* filename, int maxDepth, bool quietErrors);

} //XR

#endif //XR_JSONUTILS_HPP
