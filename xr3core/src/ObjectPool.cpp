//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/ObjectPool.hpp"

namespace xr
{
namespace detail
{
namespace
{

//==============================================================================
std::byte* AllocatePool(IndexServer::Index numHandles, size_t objectSize)
{
  objectSize *= numHandles;
  return numHandles != 0 ?
    static_cast<std::byte*>(operator new(objectSize, std::nothrow)) : nullptr;
}

}

//==============================================================================
ObjectPoolCore::ObjectPoolCore(IndexServer::Index numHandles, size_t objectSize) noexcept:
  mBuffer{ AllocatePool(numHandles, objectSize) },
  mHandles{ mBuffer ? numHandles : 0 }
{}

//==============================================================================
ObjectPoolCore::~ObjectPoolCore() noexcept
{
  operator delete(mBuffer);
}

//==============================================================================

}
}

