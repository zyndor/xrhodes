//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Animator.hpp"
#include "xr/debug.hpp"
#include <memory>
#include <algorithm>
#include <functional>
#include <cstring>

namespace xr
{

//#define ANIMATOR_MEMORY_DEBUG

//==============================================================================
void Animator::ValueBase::Update(float tDelta)
{
  float progress = std::min(mProgress + mInvDuration * tDelta, 1.f);
  mProgress = progress;

  UpdateValue(progress);
  if (progress >= 1.f)
  {
    if (mOnStop)
    {
      mOnStop->Call();
      mOnStop.reset();
    }
  }
}

//==============================================================================
void Animator::ValueBase::Complete()
{
  Update((1.0f - mProgress) * mInvDuration);
}

//==============================================================================
Animator::Animator(size_t initSize)
{
  for (auto& rs: mValueBuffers)
  {
    rs.reserve(initSize);
  }
}

//==============================================================================
Animator::~Animator()
{
  Clear(false);
}

//==============================================================================
bool Animator::Stop(Handle handle, bool complete)
{
  for (auto& vb: mValueBuffers)
  {
    if (auto value = Find(vb, handle))
    {
      if (complete)
      {
        value->Complete();
      }

      value->~ValueBase();

      struct DeadValue : public ValueBase
      {
        DeadValue(Handle handle)
        : ValueBase(handle, 1e-6f, nullptr)
        {}

        void UpdateValue(float) override
        {}
      };
      new (value) DeadValue(handle);
      return true;
    }
  }

  return false;
}

//==============================================================================
void Animator::Update(float tDelta)
{
  XR_ASSERT(Animator, !mIsTraversing);
  mIsTraversing = true;
  std::unique_ptr<bool, std::function<void(bool*)>> traversingGuard(&mIsTraversing,
    [&](bool* p) {
      *p = false;
      RemoveExpired();
      Merge();
    });

  auto& valueBuffer = mValueBuffers[0];
  auto i0 = valueBuffer.data();
  auto i1 = i0 + valueBuffer.size();
  while (i0 != i1)
  {
    auto value = reinterpret_cast<ValueBase*>(i0);
    i0 += value->GetSize();
    value->Update(tDelta);
  }
}

//==============================================================================
void Animator::Clear(bool complete)
{
  XR_ASSERT(Animator, !mIsTraversing);
  Merge();

  if (complete)
  {
    mIsTraversing = true;
    std::unique_ptr<bool, void(*)(bool*)> traversingGuard(&mIsTraversing, [](bool* p) {
      *p = false;
    });
    auto i0 = mValueBuffers[0].data();
    auto i1 = i0 + mValueBuffers[0].size();
    while (i0 != i1)
    {
      auto value = reinterpret_cast<ValueBase*>(i0);
      i0 += value->GetSize(); // NOTE: must precede:
      value->Complete();
      value->~ValueBase();
    }
  }
  mValueBuffers[0].clear();
}

//==============================================================================
void * Animator::Allocate(uint32_t size)
{
  auto& valueBuffer = mValueBuffers[mIsTraversing];
  size_t used = valueBuffer.size();
  while (used + size > valueBuffer.capacity())
  {
    valueBuffer.reserve(valueBuffer.capacity() * 2);
#ifdef ANIMATOR_MEMORY_DEBUG
    XR_TRACE(Animator, ("Reserving more space, %d bytes total", valueBuffer.capacity()));
#endif
  }

  auto iInsert = valueBuffer.insert(valueBuffer.end(), size, 0);
#ifdef ANIMATOR_MEMORY_DEBUG
  XR_TRACE(Animator, ("Allocating %d bytes; %d so far. Next key: %d", size,
    valueBuffer.size(), mNextKey));
#endif
  return &*iInsert;
}

//==============================================================================
void Animator::RemoveExpired()
{
  XR_ASSERT(Animator, !mIsTraversing);

  // removal of expired entries
  // - start offset at 0 bytes
  // - find block of expired entries (>0), increase offset by block size (bytes)
  // - find next expired entry (or end of buffer)
  // - move entries down by recorded byte size
  auto& valueBuffer = mValueBuffers[0];
#ifdef ANIMATOR_MEMORY_DEBUG
  const bool doDump = !valueBuffer.empty();

  auto dumpValues = [this]()
  {
    auto& valueBuffer = mValueBuffers[0];
    XR_TRACEIF(Animator, !valueBuffer.empty(), ("Buffer size: %d", valueBuffer.size()));
    auto i0 = valueBuffer.data();
    auto i1 = i0 + valueBuffer.size();
    while (i0 != i1)
    {
      auto value = reinterpret_cast<ValueBase const*>(i0);
      XR_TRACE(Animator, ("Prop#%d: %d bytes, progress: %.3f", value->mHandle.mKey,
        value->GetSize(), value->mProgress));
      i0 += value->GetSize();
    }
  };

  XR_TRACEIF(Animator, doDump, ("Before Remove()")); dumpValues();
#endif

  auto i0 = valueBuffer.data();
  auto i1 = i0 + valueBuffer.size();
  uint8_t* moveTarget = nullptr;
  do
  {
    uint8_t* moveSource = nullptr;
    uint8_t* moveEnd = nullptr;
    while (i0 != i1)
    {
      // The purpose of the inner loop is to:
      // 1, find the first expired value, whose address will be the target of the memmove;
      // 2, find the first non-expired value, whose address will be the source of the memmove;
      // 4, invoke destructors on every ValueBase between the two;
      // 3, find the second expired value (or end of value buffer), which will determine
      //   the size of the block being moved.
      auto value = reinterpret_cast<ValueBase*>(i0);
      const auto size = value->GetSize();
      if (value->mProgress >= 1.f)
      {
        if (!moveSource)
        {
          value->~ValueBase();
        }

        if (!moveTarget)  // start of gap
        {
          moveTarget = i0;
        }
        else if (moveSource)  // end of block
        {
          moveEnd = i0;
          break;
        }
      }
      else if (moveTarget && !moveSource) // end of gap, start of block
      {
        moveSource = i0;
      }
      i0 += size;
    }

    if (!moveEnd)  // reached the end of buffer
    {
      moveEnd = i0;
    }

    if (!moveSource)
    {
      moveSource = moveEnd;
    }

    auto blockSize = moveEnd - moveSource;
    if (blockSize > 0)
    {
#ifdef ANIMATOR_MEMORY_DEBUG
      XR_TRACE(Animator, ("gap: %d(%d), block: %d+%d", moveTarget - valueBuffer.data(),
        moveSource - moveTarget, moveSource - valueBuffer.data(), blockSize));
#endif
      std::copy(moveSource, moveSource + blockSize, moveTarget);
      moveTarget += blockSize;
      moveSource = nullptr;
      moveEnd = nullptr;
    }
  }
  while (i0 != i1);

  if (moveTarget)
  {
    size_t shrinkage = i1 - moveTarget;
    if (shrinkage > 0)  // is it possible?
    {
      valueBuffer.resize(valueBuffer.size() - shrinkage);
    }
#ifdef ANIMATOR_MEMORY_DEBUG
    XR_TRACE(Animator, ("Removed %d bytes, current size: %d bytes.", shrinkage, valueBuffer.size()));
#endif
  }

#ifdef ANIMATOR_MEMORY_DEBUG
  XR_TRACEIF(Animator, doDump, ("After Remove()")); dumpValues();
#endif
}

//==============================================================================
Animator::ValueBase* Animator::Find(ValueBuffer& valueBuffer, Handle h)
{
  auto i0 = valueBuffer.data();
  auto i1 = i0 + valueBuffer.size();
  while (i0 != i1)
  {
    auto value = reinterpret_cast<ValueBase*>(i0);
    if (value->Match(h))
    {
      return value;
    }
    i0 += value->GetSize();
  }

  return nullptr;
}

//==============================================================================
void Animator::Merge()
{
  mValueBuffers[0].insert(mValueBuffers[0].end(), mValueBuffers[1].begin(), mValueBuffers[1].end());
  mValueBuffers[1].clear();
}

} // xr
