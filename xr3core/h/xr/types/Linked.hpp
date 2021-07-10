#ifndef XR_LINKED_HPP
#define XR_LINKED_HPP
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
///@brief Intrusive doubly linked list template, which allows processing every
/// one of its elements in one go. Each new instance adds itself to the end of
/// its Linked<> list upon construction, and unlinks itself upon destruction.
/// The primary intended use of Linked<> is to tie together objects that may be
/// defined in different translation units, typically as globals (file statics).
/// As such, no assumption should be made about the order of these objects in
/// the list.
/// The use of the Curiously Recurring Template Pattern is recommended, i.e.
/// derive YourClass from Linked<YourClass>.
template <typename T>
class Linked
{
public:
  // static
  ///@brief Calls @a fn with each item in the Linked<> list for this type.
  static void ForEach(std::function<void(T&)> fn);

  template <typename... Args>
  static void ForEach(std::function<void(T&, Args...)> fn, Args... args);

  ///@brief Calls T:: @a method with each item in the Linked<> list for this
  /// type, and thegiven @a args arguments.
  template <typename... Args>
  static void ForEach(void(T::*method)(Args...), Args... args);

  // structors
  Linked(T& obj);
  ~Linked();

private:
  // static
  static Linked<T>* sHead;

  // data
  T* mData;
  Linked<T>* mPrev;
  Linked<T>* mNext;
};

template <typename T>
Linked<T>* Linked<T>::sHead = nullptr;

//==============================================================================
// inline
//==============================================================================
template <typename T>
void Linked<T>::ForEach(std::function<void(T&)> fn)
{
  Linked<T>* p = sHead;
  while (p)
  {
    auto pp = p->mPrev;
    fn(*p->mData);
    p = pp;
  }
}

//==============================================================================
template <typename T>
template <typename... Args>
void Linked<T>::ForEach(std::function<void(T&, Args...)> fn, Args... args)
{
  Linked<T>* p = sHead;
  while (p)
  {
    auto pp = p->mPrev;
    fn(*p->mData, args...);
    p = pp;
  }
}

//==============================================================================
template <typename T>
template <typename... Args>
void Linked<T>::ForEach(void(T::*method)(Args...), Args... args)
{
  Linked<T>* p = sHead;
  while (p)
  {
    auto pp = p->mPrev;
    (p->mData->*method)(args...);
    p = pp;
  }
}

//==============================================================================
template<typename T>
Linked<T>::Linked(T & obj)
: mData(&obj),
  mPrev(sHead),
  mNext(nullptr)
{
  if (sHead)
  {
    sHead->mNext = this;
  }

  sHead = this;
}

//==============================================================================
template<typename T>
Linked<T>::~Linked()
{
  if (this == sHead)
  {
    sHead = mPrev;
  }

  if (mPrev)
  {
    mPrev->mNext = mNext;
  }

  if (mNext)
  {
    mNext->mPrev = mPrev;
  }
}

}

#endif //XR_LINKED_HPP
