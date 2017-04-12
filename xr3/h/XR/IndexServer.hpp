//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_INDEXSERVER_HPP
#define XR_INDEXSERVER_HPP

#include "debug.hpp"
#include <vector>

namespace XR
{

//==============================================================================
///@brief Responsible for the bookkeeping of indices, supposedly into an array
/// used as a pool of preallocated objects, that one can acquire and release
/// as they would acquire and release object( reference)s, but in an optimal
/// and efficient way.
class IndexServer
{
public:
  // structors
  explicit IndexServer(size_t capacity);

  // general
  ///@return A valid index. (This will either be unused or has been Release()d.
  ///@note Throws std::bad_alloc if the capacity of the server has been exceeded.
  size_t  Acquire();

  ///@brief Returns the given index to the pool.
  ///@note Throws std::logic_error if the index has already been released.
  void    Release(size_t i);

private:
  // data
  std::vector<size_t> m_ids;
  size_t m_iReleased; // indicates the end of the active (allocated) indices' block.
};

}

#endif //XR_ALLOCATIONTRACKER_HPP

