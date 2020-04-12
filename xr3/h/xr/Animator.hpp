#ifndef XR_ANIMATOR_HPP
#define XR_ANIMATOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/events/Callback.hpp"
#include "xr/memory/memory.hpp"
#include "xr/types/fundamentals.hpp"
#include <vector>
#include <algorithm>
#include <memory>

namespace xr
{

//==============================================================================
///@brief Manages animations of values which custom interpolation and custom
/// setters may be provided for.
class Animator
{
  XR_NONCOPY_DECL(Animator)

public:
  // types
  ///@brief Interpolates between source and target values, given the progress along
  /// the animation (as [0..1]).
  template <typename T>
  using FnTween = T(*)(T source, T target, float alpha);

  ///@brief Sets the updated value on the target.
  template <typename T>
  using FnSet = xr::Callback<void, T>;

  ///@brief An operation performed upon the completion of the animation.
  using OnStop = xr::Callback<void>;

  using Size = uint16_t;
  using Key = uint16_t;

  ///@brief Identifier of an animation, which can be used to Stop() it.
  struct Handle
  {
    Size mSize; // size 0 indicates an invalid key.
    Key mKey;
  };

  // structors
  explicit Animator(size_t valueBufferSize = XR_KBYTES(2));
  ~Animator();

  // general
  ///@brief Starts an animation, to run for @a duration units of time, interpolating
  /// with the use of the given @a tween, between the @a start and @a end values,
  /// setting the result with @a setter. An optional @a onStop callback may be
  /// provided, to be called when the animation is stopped for any reason.
  ///@return Handle to the animation. This may be used for manually Stop()ping
  /// the animation.
  template <typename T>
  Handle Animate(float duration, T const& start, T const& target, FnTween<T> tween,
    FnSet<T> const& setter, OnStop const* onStop = nullptr);

  ///@brief Attempts to stop an animation with the given @a handle. If @a complete
  /// is true, the value will be set to its target and the onStop callback will
  /// be called (if one was provided for the animation).
  ///@return Whether an animation with the given @a handle could be found.
  bool Stop(Handle handle, bool complete);

  ///@brief Simulates the passage of @a tDelta units of time, updating all animations.
  /// The ones that reach their target duration are removed, and if they had an
  /// onStop function specified, it's called.
  void Update(float tDelta);

  ///@brief Removes all animations. If @a complete is true, their values will be
  /// set to their target and their onStop callback will be called (for those that
  /// had it provided).
  void Clear(bool complete);

private:
  // types
  struct ValueBase
  {
    const Handle mHandle;
    const float mInvDuration;
    float mProgress;
    std::unique_ptr<OnStop> mOnStop;

    ValueBase(Handle handle, float duration, OnStop const* onStop)
    : mHandle(handle),
      mInvDuration(1.f / duration),
      mProgress(0.f),
      mOnStop(onStop ? onStop->Clone() : nullptr)
    {}

    virtual ~ValueBase() {}

    bool Match(Handle const& h) const
    {
      return h.mSize == mHandle.mSize && h.mKey == mHandle.mKey;
    }

    Size GetSize() const
    {
      return mHandle.mSize;
    }

    void Update(float tDelta);
    void Complete();

  protected:
    virtual void UpdateValue(float alpha) =0;
  };

  using ValueBuffer = std::vector<uint8_t>;

  // data
  ValueBuffer mValueBuffers[2];
  bool mIsTraversing = false;
  Key mNextKey = 0;

  // internal
  void* Allocate(uint32_t bytes);

  ValueBase* Find(ValueBuffer& vb, Handle h);

  void RemoveExpired();
  void Merge();
};

//==============================================================================
// inline
//==============================================================================
template<typename T>
Animator::Handle Animator::Animate(float duration, T const& start, T const& target,
   FnTween<T> tween, FnSet<T> const& setter, OnStop const* onStop)
{
  if (duration > 0.f)
  {
    using RawT = typename std::decay<T>::type;
    struct Value : ValueBase
    {
      RawT mStart;
      RawT mTarget;
      FnTween<T> mTween;
      std::unique_ptr<FnSet<T>> mSetter;

      Value(Key key, float duration, FnTween<T> tween, FnSet<T> const& setter,
        T const& start, T const& target, OnStop const* onStop)
      : ValueBase(Handle { sizeof(Value), key }, duration, onStop),
        mStart(start),
        mTarget(target),
        mTween(tween),
        mSetter(setter.Clone())
      {}

      virtual void UpdateValue(float alpha) override
      {
        mSetter->Call(mTween(mStart, mTarget, alpha));
      }
    };

    void* buffer = Allocate(sizeof(Value));
    XR_ASSERT(Animator, buffer);

    auto result = new (buffer) Value(mNextKey, duration, tween, setter, start,
      target, onStop);
    ++mNextKey;
    return result->mHandle;
  }
  else
  {
    setter.Call(target);
    if (onStop)
    {
      onStop->Call();
    }
    return{ 0, 0 };
  }
}

} // xr

#endif //XR_ANIMATOR_HPP
