//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/ShaderComponent.hpp"
#include "xr/BufferReader.hpp"
#include "xr/streamutils.hpp"
#include <map>

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
    XR_ASSERT(Asset::Manager, ext);
    ++ext;

    auto iFind = m_types.find(Hash::String32(ext));
    XR_ASSERT(ShaderComponentBuilder, iFind != m_types.end());

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
  Gfx::Release(m_handle);
  auto p = reinterpret_cast<uint8_t const*>(source);
  m_handle = Gfx::CreateShader(type, { strlen(source), p });

  bool success = m_handle.IsValid();
  if(success)
  {
    m_type = type;

    if(CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
    {
      m_data.assign(p, p + strlen(source));
    }
  }
  return success;
}

//==============================================================================
bool ShaderComponent::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);

  bool success = reader.Read(m_type);
  if (success)
  {
    auto sourceLen = reader.GetRemainingSize();
    auto source = reader.ReadBytes(sourceLen);
    success = source != nullptr;
    if (
#ifdef ENABLE_ASSET_BUILDING
      !CheckAllMaskBits(GetFlags(), DryRunFlag) &&
#endif
      success)
    {
      m_handle = Gfx::CreateShader(m_type, { sourceLen, source });
      success = m_handle.IsValid();
    }

    if (success && CheckAllMaskBits(GetFlags(), KeepSourceDataFlag))
    {
      m_data.assign(buffer.data, buffer.data + buffer.size);
    }
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

}
