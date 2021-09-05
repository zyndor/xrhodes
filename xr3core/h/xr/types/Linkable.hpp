#ifndef XR_LINKABLE_HPP
#define XR_LINKABLE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <functional>

namespace xr
{

//==============================================================================
///@brief Intrusive doubly linked list template, offering facilities to process every
/// one of its linked elements in one go.
template <typename T>
struct Linkable
{
  // static
  ///@brief Calls @a method with each linked instance of Linkable<T>, with the given
  /// arguments @a args.
  template <typename... Args>
  static void ForEach(void(T::*method)(Args...), Args... args);

  ///@brief Calls the function @a fn, passing each linked instance of Linkable<T> to it.
  static void ForEach(std::function<void(T&)> fn);

  ///@brief Calls the function @a fn, passing each linked instance of Linkable<T>,
  /// and the given arguments @a args to it.
  template <typename... Args>
  static void ForEach(std::function<void(T&, Args...)> fn, Args... args);

  // structors
  Linkable(T& data) noexcept;
  ~Linkable() noexcept;

protected:
  // internal
  ///@brief Links this to the list.
  void Link() noexcept;

  ///@brief Unlinks this from the list.
  void Unlink() noexcept;

  ///@brief Obtains the data that this was created with.
  ///@note No ownership transfer.
  T* GetData() const noexcept;

private:
  // static
  static Linkable<T>* sTail;

  // data
  T* mData; // no ownership
  Linkable<T>* mPrev{ nullptr };
  Linkable<T>* mNext{ nullptr };
};

//==============================================================================
// inline
//==============================================================================
template <typename T>
Linkable<T>* Linkable<T>::sTail = nullptr;

//==============================================================================
template<class T>
template<typename ...Args>
inline
void Linkable<T>::ForEach(void(T::* method)(Args...), Args ...args)
{
  auto i = sTail;
  while (i)
  {
    auto prev = i->mPrev;
    (i->mData->*method)(args...);
    i = prev;
  }
}

//==============================================================================
template<class T>
inline
void Linkable<T>::ForEach(std::function<void(T&)> fn)
{
  auto i = sTail;
  while (i)
  {
    auto prev = i->mPrev;
    fn(*i->mData);
    i = prev;
  }
}

//==============================================================================
template<class T>
template<typename ...Args>
inline
void Linkable<T>::ForEach(std::function<void(T&, Args...)> fn, Args ...args)
{
  auto i = sTail;
  while (i)
  {
    auto prev = i->mPrev;
    fn(*i->mData, args...);
    i = prev;
  }
}

//==============================================================================
template<class T>
Linkable<T>::Linkable(T& data) noexcept
: mData(&data)
{}

//==============================================================================
template<class T>
Linkable<T>::~Linkable() noexcept
{
  Unlink();
}

//==============================================================================
template<class T>
void Linkable<T>::Link() noexcept
{
  if (mPrev || mNext)
  {
    return; // already linked
  }

  if (sTail)
  {
    sTail->mNext = this;
    mPrev = sTail;
    sTail = this;
  }
  else
  {
    sTail = this;
  }
}

//==============================================================================
template<class T>
inline void Linkable<T>::Unlink() noexcept
{
  if (mPrev)
  {
    mPrev->mNext = mNext;
  }

  if (mNext)
  {
    mNext->mPrev = mPrev;
  }
  else if (sTail == this)
  {
    sTail = mPrev;
  }

  mPrev = nullptr;
  mNext = nullptr;
}

//==============================================================================
template<class T>
inline
T* Linkable<T>::GetData() const noexcept
{
  return mData;
}

}

#endif //XR_LINKABLE_HPP
