#ifndef ASSETQUEUE_HPP
#define ASSETQUEUE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Asset.hpp"
#include "xr/memory/Queue.hpp"

// AssetQueue helps facilitate the process of loading and reloading several
// assets.
class AssetQueue
{
public:
  template <typename T>
  void Add(xr::Counted<T>& asset)
  {
    m_assets.push_back(Asset::Ptr(asset.Get()));
  }

  void Update()
  {
    using namespace xr;
    Asset::Manager::Update();
    if (!m_assets.empty())
    {
      auto asset = m_assets.front();
      auto flags = asset->GetFlags();
      if(CheckAllMaskBits(flags, Asset::ErrorFlag))
      {
        auto descriptor = asset->GetDescriptor();
        std::ostringstream stream;
        stream << "Failed to load: " << descriptor.hash << " (" <<
          std::string(reinterpret_cast<char*>(&descriptor.type), 4) << ").";
        throw std::runtime_error(stream.str());
      }
      else if(CheckAllMaskBits(flags, Asset::ReadyFlag))
      {
        m_reloadables.push_back(asset->GetDescriptor());
        m_assets.pop_front();
      }
    }
  }

  bool  IsReady() const
  {
    return m_assets.empty();
  }

  void Reload()
  {
    using namespace xr;
    for (auto i: m_reloadables)
    {
      m_assets.push_back(Asset::Manager::LoadReflected(i, Asset::ForceReloadFlag));
    }
    m_reloadables.clear();
  }

private:
  xr::Queue<xr::Asset::Ptr> m_assets;
  xr::Queue<xr::Asset::DescriptorCore> m_reloadables;
};

#endif
