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
#include "ParseAssetOptions.hpp"
#include "xr/io/streamutils.hpp"
#include <map>
#include <regex>
#endif

#define LTRACE(format) XR_TRACE(ShaderComponent, format)
#define LTRACEIF(condition, format) XR_TRACEIF(ShaderComponent, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(ShaderComponent, "xshc", 2, "vsh;fsh")

namespace
{

#ifdef ENABLE_ASSET_BUILDING
const std::regex kDefineRegex("[[:alpha:]_][[:alnum:]_]*");

const std::regex kEolRegex("\\r\\n?");
const std::string kEolReplace("\n");

const std::regex kCommentsRegex(
  "(//.*|" // C++-style comments
  "/\\*.*\\n(.*\\n)*.*\\*/)+"  // C-style comments
);

const std::regex kHeadingWhiteSpaceRegex("(^|\\n)\\s+");
const std::regex kTrailingWhiteSpaceRegex("\\s+($|\\n)");

const std::regex kPreprocessorRegex("^#.*\\s*");

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
    const auto definesBlock = [rawNameExt](){
      std::ostringstream stream;
      ParseAssetOptions(rawNameExt, [rawNameExt, &stream](char const* option) {
        bool result = std::regex_match(option, kDefineRegex);
        if (result)
        {
          stream << "#define " << option << std::endl;
        }
        else
        {
          LTRACE(("%s: invalid option '%s'", rawNameExt, option));
        }
        return result;
      });
      return stream.str();
    }();

    auto ext = strchr(rawNameExt, '.');
    XR_ASSERT(ShaderComponent, ext);
    ++ext;

    auto extHash = Hash::String32(ext, strcspn(ext, Asset::kOptionDelimiter));
    auto iFind = m_types.find(extHash);
    XR_ASSERT(ShaderComponent, iFind != m_types.end());

    // Remove comments and whitespace bloat from source.
    std::string source(buffer.As<char const>(), buffer.size);
    source = std::regex_replace(source, kEolRegex, kEolReplace);
    source = std::regex_replace(source, kCommentsRegex, "");
    source = std::regex_replace(source, kHeadingWhiteSpaceRegex, "$1");
    source = std::regex_replace(source, kTrailingWhiteSpaceRegex, "$1");

    // Add defines
    if (!definesBlock.empty())
    {
      std::ostringstream stream;
      std::cmatch preprocResults;
      char const* p = source.c_str();
      while (std::regex_search(p, preprocResults, kPreprocessorRegex) &&
        preprocResults[0].first == p)
      {
        auto pNew = preprocResults[0].second;
        while (p != pNew)
        {
          stream.put(*p);
          ++p;
        }
      }

      stream << definesBlock;
      stream << p;
      source = stream.str();
    }

    // TODO: support includes
    return WriteBinaryStream(iFind->second, data) &&
      data.write(source.c_str(), source.size());
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
