//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "jsonutils.hpp"
#include "JsonReader.hpp"
#include "FileBuffer.hpp"
#include <XR/debug.hpp>

namespace XR
{

//==============================================================================
JSON::Entity* LoadJSON(const char * pFilename, int maxDepth, bool quietErrors)
{
  XR_ASSERT(LoadJSON, pFilename != nullptr);
  XR::FileBuffer  file;
  if (!file.Open(pFilename, "rb"))
  {
    if (!quietErrors)
    {
      XR_ERROR(("Failed to open file '%s'", pFilename));
    }
    return nullptr;
  }

  file.Close();

  // parse parse parse
  JSON::Reader  reader(maxDepth);
  JSON::Entity* pJson(reader.Read(file.GetData(), file.GetSize()));

  // done, clean up
  file.Destroy();
  if (pJson == nullptr && !quietErrors)
  {
    XR_ERROR(("Failed to parse '%s': error around row %d char %d",
      pFilename, reader.GetState().GetRow(), reader.GetState().GetColumn()));
  }

  return pJson;
}

} // XR