#ifndef XR_INDEXSERVER_HPP
#define XR_INDEXSERVER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/fundamentals.hpp"
#include "xr/debug.hpp"
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Responsible for the bookkeeping of indices, supposedly into an array
/// used as a pool of preallocated objects, that one can acquire and release
/// as they would acquire and release object( reference)s, but in an optimal
/// and efficient way.
class IndexServer
{
  XR_NONCOPY_DECL(IndexServer)

public:
  // types
  using Index = uint32_t;

  // static
  static constexpr Index kInvalidIndex = Index(-1);

  // structors
  explicit IndexServer(Index capacity) noexcept;
  ~IndexServer() noexcept;

  // general
  ///@return An index, which is either unused or has been Release()d. If the IndexServer
  /// has run out of indices, it will return kInvalidIndex.
  [[nodiscard]] Index  Acquire() noexcept;

  ///@brief Returns the given index @a i to the pool.
  ///@return Whether the operation was successful - which it will only be if the
  /// index @a i has been previously Acquire()d and has not yet been Release()d.
  bool  Release(Index i) noexcept;

  ///@return Number of indices ever Acquire()d at any given time (even if they have
  /// been since Release()d).
  Index GetNumActive() const noexcept;

  ///@return Number of Acquire()d indices. Useful for leaks checking.
  Index GetNumAcquired() const noexcept;

  ///@return Maximum indices that we can serve.
  Index GetCapacity() const noexcept;

private:
  // data
  Index* mIds;
  Index mReleased{ 0 }; // indicates the end of the active (allocated) indices' block.
  Index mLast{ 0 }; // indicates the end of the used indices' block.
  Index mCapacity{ 0 };
};

}

#endif //XR_ALLOCATIONTRACKER_HPP

