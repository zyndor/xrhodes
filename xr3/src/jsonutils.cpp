//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/jsonutils.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/JsonReader.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
JSON::Entity* LoadJSON(const char* filename, int maxDepth, bool quietErrors)
{
  XR_ASSERT(LoadJSON, filename != nullptr);
  FileBuffer  file;
  if (!file.Open(filename, false))
  {
    if (!quietErrors)
    {
      XR_ERROR(("Failed to open file '%s'", filename));
    }
    return nullptr;
  }

  file.Close();

  // parse parse parse
  JSON::Reader  reader(maxDepth);
  JSON::Entity* pJson = reader.Read(file.CastData<char>(), file.GetSize());

  // done, clean up
  file.Destroy();
  if (pJson == nullptr && !quietErrors)
  {
    XR_ERROR(("Failed to parse '%s': error around row %d char %d",
      filename, reader.GetState().GetRow(), reader.GetState().GetColumn()));
  }

  return pJson;
}

} // xr
