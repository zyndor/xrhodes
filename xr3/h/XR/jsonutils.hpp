//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_JSONUTILS_HPP
#define XR_JSONUTILS_HPP

#include "XR/JsonEntity.hpp"

namespace XR
{

//==============================================================================
///@brief Attempts to load a json file under the given path, and process up
/// to @a maxDepth levels. @a quietErrors defines whether errors encountered
/// during processing should be flagged.
///@return Pointer to and ownership of the root element of the document. If
/// an error has occurred, this will be nullptr.
JSON::Entity* LoadJSON(const char* pFilename, int maxDepth, bool quietErrors);

} //XR

#endif //XR_JSONUTILS_HPP