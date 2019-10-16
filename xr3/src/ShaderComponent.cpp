//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ShaderComponent.hpp"
#include "xr/memory/BufferReader.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "xr/io/streamutils.hpp"
#include <map>
#include <regex>
#endif

#define LTRACE(format) XR_TRACE(ShaderComponent, format)
#define LTRACEIF(condition, format) XR_TRACEIF(ShaderComponent, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(ShaderComponent, "xshc", 1, "vsh;fsh")

namespace
{

#ifdef ENABLE_ASSET_BUILDING

class ShaderComponentBuilder : public Asset::Builder
{
public:
  ShaderComponentBuilder()
  : Asset::Builder(ShaderComponent::kTypeId)
  {
    m_types[Hash::String32("vsh")] = Gfx::ShaderType::Vertex;
    m_types[Hash::String32("fsh")] = Gfx::ShaderType::Fragment;
  }

  bool Build(char const* rawNameExt, Buffer buffer,
    std::vector<FilePath>& dependencies, std::ostream& data) const override
  {
    auto ext = strrchr(rawNameExt, '.');
    XR_ASSERT(ShaderComponent, ext);
    ++ext;

    auto iFind = m_types.find(Hash::String32(ext));
    XR_ASSERT(ShaderComponent, iFind != m_types.end());

    // TODO: support includes
    return WriteBinaryStream(iFind->second, data) &&
      data.write(buffer.As<char const>(), buffer.size);
  }

private:
  std::map<uint32_t, Gfx::ShaderType> m_types;
} shaderComponentBuilder;
#endif

}

//==============================================================================
bool ShaderComponent::SetSource(Gfx::ShaderType type, char const* source)
{
  XR_DEBUG_ONLY(auto flags = GetFlags());
  XR_ASSERT(ShaderComponent, CheckAllMaskBits(flags, ErrorFlag) ||
    !CheckAnyMaskBits(flags, LoadingFlag | ProcessingFlag));

  Gfx::Release(m_handle);
  auto p = reinterpret_cast<uint8_t const*>(source);
  const bool success = SetSourceInternal(type, { strlen(source), p });

  OverrideFlags(PrivateMask, success ? ReadyFlag : (ProcessingFlag | ErrorFlag));

  return success;
}

//==============================================================================
bool ShaderComponent::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  Gfx::ShaderType type;
  bool success = reader.Read(type);
  if (success)
  {
    auto sourceLen = reader.GetRemainingSize();
    auto source = reader.ReadBytes(sourceLen);
    success = source != nullptr;
    if (success)
    {
#ifdef ENABLE_ASSET_BUILDING
      if (!CheckAllMaskBits(GetFlags(), DryRunFlag))
      {
#endif
      success = SetSourceInternal(type, { sourceLen, source });
#ifdef ENABLE_ASSET_BUILDING
      }
#endif
    }
    else
    {
      LTRACE(("%s: failed to read source.", m_debugPath.c_str()));
    }
  }
  else
  {
    LTRACE(("%s: failed to read type.", m_debugPath.c_str()));
  }

  return success;
}

//==============================================================================
void xr::ShaderComponent::OnUnload()
{
  Gfx::Release(m_handle);
  m_handle.Invalidate();

  std::vector<uint8_t>().swap(m_data);
}

//==============================================================================
bool ShaderComponent::SetSourceInternal(Gfx::ShaderType type, Buffer buffer)
{
  auto handle = Gfx::CreateShader(type, buffer);
  bool success = handle.IsValid();
  if (success)
  {
    m_handle = handle;
    m_type = type;

    if (CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
    {
      m_data.assign(buffer.data, buffer.data + buffer.size);
    }
  }
  else
  {
    LTRACE(("%s: Shader compilation has failed; see errors above.", m_debugPath.c_str()));
  }

  return success;
}

}
