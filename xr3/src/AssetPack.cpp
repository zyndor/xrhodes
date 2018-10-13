//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/AssetPack.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/io/streamutils.hpp"

#define LTRACE(format) XR_TRACE(AssetPack, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(AssetPack, "aspk", 1, "pak")

//==============================================================================
namespace
{

using AssetCountType = uint32_t;
using AssetSizeType = uint32_t;

struct AssetHeader
{
  Asset::HashType hash;
  Asset::TypeId typeId;
  Asset::VersionType version;
  bool hasAlias;
};

#ifdef ENABLE_ASSET_BUILDING
Asset::Builder const* FindBuilder(Asset::TypeId typeId)
{
  Asset::Builder const* builder = nullptr;
  auto findBuilder = [&typeId, &builder](Asset::Builder const& b) {
    if (b.type == typeId)
    {
      builder = &b;
    }
  };

  Asset::Builder::Base::ForEach(findBuilder);
  return builder;
}

static class AssetPackBuilder: public Asset::Builder
{
public:
  // binary format
  // (4 bytes - fourcc - aspk
  // 2 bytes - version
  // 2 bytes - reserved)
  // 4 bytes - number of assets
  // ( sizeof(AssetHeader) bytes - header
  // 8 bytes - alias hash
  // 4 bytes - asset size
  // asset size bytes - asset ) * number of assets
  AssetPackBuilder()
  : Builder(AssetPack::kTypeId)
  {}

  bool Build(char const* rawNameExt, Buffer buffer, std::vector<FilePath>& dependencies,
    std::ostream& data) const override
  {
    XonParser::State state;
    std::unique_ptr<XonObject> root(XonBuildTree(buffer.As<char const>(), buffer.size, &state));
    if (!root)
    {
      LTRACE(("%s: failed to parse XON somewhere around row %d, column %d.", rawNameExt,
          state.row, state.column));
      return false;
    }

    // Write number of assets
    const size_t numAssets = root->GetNumElements();
    if (!WriteBinaryStream(static_cast<AssetCountType>(numAssets), data))
    {
      LTRACE(("%s: Failed to write number of assets.", rawNameExt));
      return false;
    }

    // Process aliases
    std::map<Asset::HashType, std::string> aliasMap;
    try
    {
      std::vector<std::string> aliases;
      root->GetKeys(aliases);
      for (auto& a : aliases)
      {
        auto value = root->Get(a).ToValue().GetString();
        aliasMap[Asset::Manager::HashPath(value)] = std::move(a);
      }
    }
    catch (XonEntity::Exception const& e)
    {
      LTRACE(("%s: error processing aliases: %s", rawNameExt, e.what()));
      return false;
    }

    for (size_t i = 0; i < numAssets; ++i)
    {
      auto& entry = (*root)[i];

      // Pre-process path.
      FilePath path(File::StripRoots(entry.ToValue().GetString()));
      if (path.StartsWith(Asset::Manager::GetAssetPath()))
      {
        path = FilePath(path.c_str() + Asset::Manager::GetAssetPath().size());
      }

      // Dig up reflector and builder.
      auto ext = path.GetExt();
      if (!ext) // TODO: support for built assets?
      {
        LTRACE(("%s: unable to determine reflector for entry '%s' without extension.",
          rawNameExt, path.c_str()));
        return false;
      }

      auto reflector = detail::AssetReflector::GetReflector(ext);
      if (!reflector) // TODO: support for built assets?
      {
        LTRACE(("%s: failed to find reflector for '%s'.", rawNameExt, path.c_str()));
        return false;
      }

      auto builder = FindBuilder(reflector->type);
      XR_ASSERT(AssetPack, builder);

      // Get alias, if any.
      auto hash = Asset::Manager::HashPath(path);
      auto iFindAlias = aliasMap.find(hash);
      bool hasAlias = iFindAlias != aliasMap.end();
      Asset::HashType alias = 0;
      if (hasAlias)
      {
        alias = Hash::String(iFindAlias->second.c_str());
      }

      // Write asset header.
      AssetHeader header = {
        hash,
        reflector->type,
        reflector->version,
        hasAlias
      };

      if (!WriteBinaryStream(header, data))
      {
        LTRACE(("%s: Failed to write header for '%s'.", rawNameExt, path.c_str()));
        return false;
      }

      // If an alias was defined, write it to the stream now.
      if (hasAlias && !WriteBinaryStream(alias, data))
      {
        LTRACE(("%s: Failed to write alias for '%s'.", rawNameExt, path.c_str()));
        return false;
      }

      // Read file (synchronously).
      FileBuffer assetBuffer;
      if (!assetBuffer.Open(path))
      {
        LTRACE(("%s: Failed to load asset from '%s'.", rawNameExt, path.c_str()));
        return false;
      }

      // Build asset's dependencies and data into temporaries.
      std::vector<FilePath> tmpDependencies;
      std::ostringstream tmpData;
      if(!builder->Build(path.c_str(), { assetBuffer.GetSize(), assetBuffer.GetData() },
        tmpDependencies, tmpData))
      {
        LTRACE(("%s: Failed to build asset from '%s'.", rawNameExt, path.c_str()));
        return false;
      }

      // Write asset to package.
      auto assetData = tmpData.str();
      if (assetData.size() > std::numeric_limits<AssetSizeType>::max())
      {
        LTRACE(("%s: Asset '%s' too large; size must fit into 32 bits.", rawNameExt, path.c_str()));
      }

      AssetSizeType assetSize = static_cast<AssetSizeType>(assetData.size());
      if (!WriteBinaryStream(assetSize, data) ||
        !data.write(assetData.c_str(), assetSize))
      {
        LTRACE(("%s: Failed to write asset '%s' to package.", rawNameExt, path.c_str()));
        return false;
      }

      // Insert dependencies into main one, eliminating duplicates.
      for (auto& d: tmpDependencies)
      {
        auto iInsert = std::lower_bound(dependencies.begin(), dependencies.end(), d);
        if (iInsert == dependencies.end() || d != *iInsert)
        {
          dependencies.insert(iInsert, d);
        }
      }
    }

    return true;
  }

} assetPackBuilder;
#endif

} // nonamespace

//==============================================================================
Asset::Ptr AssetPack::Get(Asset::HashType alias) const
{
  Asset::Ptr result;
  auto iFind = m_aliased.find(alias);
  if (iFind != m_aliased.end())
  {
    result = iFind->second;
  }
  return result;
}

//==============================================================================
Asset::Ptr AssetPack::Get(const char * alias) const
{
  return Get(Hash::String(alias));
}

//==============================================================================
bool AssetPack::OnLoaded(Buffer buffer)
{
  auto flags = GetFlags();

  BufferReader  reader(buffer);
  AssetCountType numAssets;
  if (!reader.Read(numAssets))
  {
    LTRACE(("%s: Failed to read state flags.", m_debugPath.c_str()));
    return false;
  }

  for (decltype(numAssets) i = 0; i < numAssets; ++i)
  {
    AssetHeader header;
    if (!reader.Read(header))
    {
      LTRACE(("%s: Failed to read header %d.", m_debugPath.c_str(), i));
      return false;
    }

    HashType alias;
    if (header.hasAlias && !reader.Read(alias))
    {
      LTRACE(("%s: Failed to read alias for asset %d.", m_debugPath.c_str(), i));
      return false;
    }

    AssetSizeType assetSize;
    if (!reader.Read(assetSize))
    {
      LTRACE(("%s: Failed to read asset %d size.", m_debugPath.c_str(), i));
      return false;
    }

    auto assetData = reader.ReadBytes(assetSize);
    if (!assetData)
    {
      LTRACE(("%s: Failed to read asset %d data.", m_debugPath.c_str(), i));
      return false;
    }

    // Create asset. Note that though it isn't managed initially, we don't want
    // to set UnmanagedFlag, since that's liable to make dependencies also not
    // managed, which might not be wanted - except if the flag was set on the
    // AssetPack by client code.
    Asset::Ptr asset(Asset::Reflect(header.typeId, header.hash, flags));
    if (!asset->ProcessData({ assetSize, assetData }))
    {
      LTRACE(("%s: Failed to process asset %d.", m_debugPath.c_str(), i));
      return false;
    }

    Manager::Manage(asset);

    if (header.hasAlias)
    {
      auto iFind = m_aliased.find(alias);
      if (iFind != m_aliased.end())
      {
        LTRACE(("%s: hash clash: alias for %lld overwrites %lld.", m_debugPath.c_str(),
          iFind->second->GetDescriptor().hash, asset->GetDescriptor().hash));
      }
      m_aliased.insert(iFind, { alias, asset });
    }
    else
    {
      m_unnamed.push_back(asset);
    }
  }

  return true;
}

//==============================================================================
void AssetPack::OnUnload()
{
  m_aliased.clear();
  m_unnamed.clear();
}

}
