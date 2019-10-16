//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/AssetPack.hpp"
#include "xr/memory/BufferReader.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "xr/xon/XonBuildTree.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/io/streamutils.hpp"
#include <unordered_map>
#endif

#define LTRACE(format) XR_TRACE(AssetPack, format)
#define LTRACEIF(cond, format) XR_TRACEIF(AssetPack, cond, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(AssetPack, "aspk", 2, "pak")

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
  // types
  struct Record
  {
    using Ptr = std::unique_ptr<Record>;
    using Vector = std::vector<Ptr>;

    AssetHeader header;

    Asset::HashType alias;
    FilePath path;
    std::vector<FilePath> dependencies;
    std::ostringstream data;
    bool isContent; // it was defined in the pack

    uint32_t maxDepth;
    uint32_t value;
  };

  using AliasMap = std::unordered_map<Asset::HashType, std::string>;

  // static
  static void NormalizePath(FilePath& path)
  {
    path = File::StripRoots(path);
    if (path.StartsWith(Asset::Manager::GetAssetPath()))
    {
      path = FilePath(path.c_str() + Asset::Manager::GetAssetPath().size());
    }

  }

  static Record::Vector::iterator FindRecord(FilePath const& path,
    Record::Vector& records)
  {
    return std::lower_bound(records.begin(), records.end(), path,
      [](Record::Ptr const& rp, FilePath const& path) {
        return path < rp->path;
      });
  }

  static Record* BuildAsset(char const* rawNameExt, AliasMap const* aliasMap,
    FilePath const& path, uint32_t depth, Record::Vector& records)
  {
    // eliminate duplicates.
    auto iFind = FindRecord(path, records);
    if (iFind != records.end() && path == (*iFind)->path)
    {
      // if we're building content, warn about already built assets.
      LTRACEIF(aliasMap, ("%s: asset '%s' is duplicate; skipping.", rawNameExt,
        path.c_str()));
      return iFind->get();
    }

    // Dig up reflector and builder.
    auto ext = path.GetExt();
    if (!ext) // TODO: support for built assets?
    {
      LTRACE(("%s: unable to determine reflector for entry '%s' without extension.",
        rawNameExt, path.c_str()));
      return nullptr;
    }

    auto reflector = detail::AssetReflector::GetReflector(ext);
    if (!reflector) // TODO: support for built assets?
    {
      LTRACE(("%s: failed to find reflector for '%s'.", rawNameExt, path.c_str()));
      return nullptr;
    }

    auto builder = FindBuilder(reflector->type);
    XR_ASSERT(AssetPack, builder);

    // Get alias, if any.
    auto hash = Asset::Manager::HashPath(path);
    bool isContent = aliasMap != nullptr;
    bool hasAlias = false;
    Asset::HashType alias = 0;
    if (isContent)
    {
      auto iFindAlias = aliasMap->find(hash);
      hasAlias = iFindAlias != aliasMap->end();
      if (hasAlias)
      {
        alias = Hash::String(iFindAlias->second.c_str());
      }
    }

    // Create record
    Record::Ptr record(new Record{
      AssetHeader{
        hash,
        reflector->type,
        reflector->version,
        hasAlias
      },
      alias,
      path,
      {},
      std::ostringstream(),
      isContent,
      depth,
      0,
    });

    // Read file (synchronously).
    FileBuffer assetBuffer;
    if (!assetBuffer.Open(path))
    {
      LTRACE(("%s: Failed to load asset from '%s'.", rawNameExt, path.c_str()));
      return nullptr;
    }

    // Build asset, gather dependencies.
    if (!builder->Build(path.c_str(), { assetBuffer.GetSize(), assetBuffer.GetData() },
      record->dependencies, record->data))
    {
      LTRACE(("%s: Failed to build asset from '%s'.", rawNameExt, path.c_str()));
      return nullptr;
    }

    // Normalize dependencies' paths
    for (auto& d : record->dependencies)
    {
      NormalizePath(d);
    }

    // Register record
    auto rawRecord = record.get();
    records.insert(iFind, std::move(record));

    return rawRecord;
  }

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
      LTRACE(("%s: failed to parse XON at row %d, column %d.", rawNameExt,
        state.row, state.column));
      return false;
    }

    // Process aliases
    std::unordered_map<Asset::HashType, std::string> aliasMap;
    try
    {
      std::vector<std::string> aliases;
      root->GetKeys(aliases);
      for (auto& a : aliases)
      {
        auto value = root->Get(a).ToValue().GetString();
        auto hash = Asset::Manager::HashPath(value);
        LTRACEIF(aliasMap.find(hash) != aliasMap.end(),
          ("%s: alias already defined for '%s', overriding with '%s'",
            rawNameExt, value, a.c_str()));
        aliasMap[hash] = std::move(a);
      }
    }
    catch (XonEntity::Exception const& e)
    {
      LTRACE(("%s: error processing aliases: %s", rawNameExt, e.what()));
      return false;
    }

    // Build all listed assets; insert into records by path.
    size_t numAssets = root->GetNumElements();
    Record::Vector records;
    records.reserve(numAssets);
    for (size_t i = 0; i < numAssets; ++i)
    {
      auto& entry = (*root)[i];

      FilePath path(entry.ToValue().GetString());
      NormalizePath(path);

      if (!BuildAsset(rawNameExt, &aliasMap, path, 0, records))
      {
        return false;
      }
    }

    // Build all dependencies; insert into records by path, and collect into
    // list of AssetPack dependencies.
    std::function<bool(FilePath const&, uint32_t)> processDependencies =
      [&processDependencies, rawNameExt, &records, &dependencies](FilePath const& path, uint32_t depth) {
        ++depth;
        auto iFind = std::lower_bound(dependencies.begin(), dependencies.end(),
          path);

        Record* record = nullptr;
        if (iFind == dependencies.end() || *iFind != path) // process uniquely
        {
          record = BuildAsset(rawNameExt, nullptr, path.c_str(), depth, records);
          if (!record)
          {
            return false;
          }

          dependencies.insert(iFind, path);
          for (auto& d: record->dependencies)
          {
            processDependencies(d, depth);
          }
        }
        else
        {
          record = FindRecord(path, records)->get();
          XR_ASSERT(AssetPack, record); // known to dependencies yet not recorded?
        }

        // update max depth and value of record.
        record->maxDepth = std::max(record->maxDepth, depth);
        ++record->value;
        return true;
      };

    std::vector<Record*> recordsView; // create view for traversal while inserting.
    recordsView.reserve(records.size());
    for (auto& rec: records)
    {
      recordsView.push_back(rec.get());
    }

    for (auto& rec: recordsView)
    {
      for (auto& d: rec->dependencies)  // check all dependencies
      {
        if (!processDependencies(d, 0))
        {
          return false;
        }
      }
    }

    // Traverse records for non-contents that have no dependencies on
    // contents. These are basically dependencies to the AssetPack itself
    // (and are removed from records).
    std::function<bool(Record const& r)> hasContentDependency =
      [&hasContentDependency, &records](Record const& rec) {
        bool result = false;
        for (auto& d: rec.dependencies)
        {
          auto iFind = FindRecord(d, records);
          if (iFind != records.end() && (*iFind)->path == d)
          {
            result = (*iFind)->isContent || hasContentDependency(**iFind);
            if (result)
            {
              break;
            }
          }
        }
        return result;
      };

    {
      auto iEnd = records.end();
      auto iRemove = std::remove_if(records.begin(), iEnd,
        [&hasContentDependency](Record::Ptr const& rec) {
          return !(rec->isContent || hasContentDependency(*rec));
        });
      records.erase(iRemove, iEnd);
    }

    // Check dependencies for paths to entries that are contents - remove these.
    {
      auto iEnd = dependencies.end();
      auto iRemove = std::remove_if(dependencies.begin(), iEnd,
        [&records](FilePath const& path) {
          auto iFind = FindRecord(path, records);
          return iFind != records.end() && path == (*iFind)->path &&
            (*iFind)->isContent;
        });
      dependencies.erase(iRemove, iEnd);
    }

    // Sort records by depth and value.
    std::sort(records.begin(), records.end(), [](Record::Ptr const& p0, Record::Ptr const& p1)
    {
      return p0->maxDepth > p1->maxDepth || (p0->maxDepth == p1->maxDepth &&
        p0->value > p1->value);
    });

    // Write number of assets, de-duplicated.
    numAssets = records.size();
    if (!WriteBinaryStream(static_cast<AssetCountType>(numAssets), data))
    {
      LTRACE(("%s: Failed to write number of assets.", rawNameExt));
      return false;
    }

    // Write assets
    for (auto& r: records)
    {
      // Asset header.
      if (!WriteBinaryStream(r->header, data))
      {
        LTRACE(("%s: Failed to write header for '%s'.", rawNameExt, r->path.c_str()));
        return false;
      }

      // If an alias was defined, write it to the stream now.
      if (r->header.hasAlias && !WriteBinaryStream(r->alias, data))
      {
        LTRACE(("%s: Failed to write alias for '%s'.", rawNameExt, r->path.c_str()));
        return false;
      }

      // Ensure that asset size fits into the serialization type.
      auto assetData = r->data.str();
      if (assetData.size() > std::numeric_limits<AssetSizeType>::max())
      {
        LTRACE(("%s: Asset '%s' too large; size must fit into 32 bits.", rawNameExt, r->path.c_str()));
      }

      AssetSizeType assetSize = static_cast<AssetSizeType>(assetData.size());
      if (!WriteBinaryStream(assetSize, data) ||
        !data.write(assetData.c_str(), assetSize))
      {
        LTRACE(("%s: Failed to write asset '%s' to package.", rawNameExt, r->path.c_str()));
        return false;
      }
    }

    return true;
  }

} assetPackBuilder;
#endif

} // nonamespace

//==============================================================================
Asset::Ptr AssetPack::GetAssetPtr(HashType alias) const
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
Asset::Ptr AssetPack::GetAssetPtr(const char * alias) const
{
  return GetAssetPtr(Hash::String(alias));
}

//==============================================================================
bool AssetPack::OnLoaded(Buffer buffer)
{
  auto flags = GetFlags() & ~Asset::UnmanagedFlag;

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
