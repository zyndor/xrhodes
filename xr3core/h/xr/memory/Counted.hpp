#ifndef XR_COUNTED_HPP
#define XR_COUNTED_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <memory>
#include <functional>

namespace xr
{

//==============================================================================
///@brief Interface for implementing intrusive reference counting.
class Countable
{
public:
  virtual ~Countable() {};

  ///@brief Requests the ownership of this to be extended by one.
  virtual void Acquire() = 0;

  ///@brief Request the ownership of this to be reduced by one. If there are no
  /// more oewners, this should release the resources acquired by the Countable.
  ///@return True if the reference count has reached zero, false otherwise.
  virtual bool Release() = 0;
};

namespace detail
{

//==============================================================================
///@brief Smart pointer type with intrusive reference counting.
class CountedCore
{
public:
  // operator overloads
  explicit operator bool() const
  {
    return m_p != nullptr;
  }

protected:
  // data
  Countable* m_p;

  // structors
  CountedCore(Countable* p)
  : m_p(p)
  {
    if (p)
    {
      p->Acquire();
    }
  }
};

} // detail

template <class T>
class Counted: protected detail::CountedCore
{
public:
  // using
  using CountedCore::operator bool;
  using Deleter = std::function<void(T*)>;

  // structors
  explicit Counted(Deleter d)
  : Counted(nullptr, d)
  {}

  Counted(std::nullptr_t p = nullptr)
  : CountedCore(p),
    m_d(std::default_delete<T>())
  {}

  explicit Counted(T* p)
  : CountedCore(p),
    m_d(std::default_delete<T>())
  {}

  Counted(T* p, Deleter d)
  : CountedCore(p),
    m_d(d)
  {}

  Counted(Counted<T> const& rhs)
  : CountedCore(rhs.m_p),
    m_d(rhs.m_d)
  {}

  Counted(Counted<T>&& rhs)
  : CountedCore(nullptr),
    m_d(nullptr)
  {
    m_p = rhs.m_p;
    rhs.m_p = nullptr;

    m_d.swap(rhs.m_d);
  }

  ~Counted()
  {
    Reset(nullptr);
  }

  // general
  ///@return A pointer to the Countable held by this.
  T* Get()
  {
    return static_cast<T*>(m_p);
  }

  ///@return A const pointer to the Countable held by this.
  T const* Get() const
  {
    return static_cast<T const*>(m_p);
  }

  ///@brief Relinquishes the ownership of the Countable held by this, to the caller,
  [[nodiscard]] T* Release()
  {
    T* p = static_cast<T*>(m_p);
    m_p = nullptr;
    return p;
  }

  ///@brief Sets the Countable held by this, to @a p.
  /// Acquire() is called on @a p (if not nullptr), and Release() is called
  /// on the previously held Countable (if any).
  void Reset(T* p)
  {
    if (p)
    {
      p->Acquire();
    }

    if (m_p && m_p->Release())
    {
      m_d(static_cast<T*>(m_p));
    }
    m_p = p;
  }

  ///@brief Sets the Countable held by this, to @a p, and the deleter to @a d.
  /// Acquire() is called on @a p (if not nullptr), and Release() is called
  /// on the previously held Countable (if any).
  template <class D = std::default_delete<T>>
  void Reset(T* p, D d = D())
  {
    Reset(p);

    m_d = d;
  }

  ///@brief Swaps pointers and deleters between this and @a other.
  void Swap(Counted<T>& other)
  {
    std::swap(m_p, other.m_p);
    m_d.swap(other.m_d);
  }

  ///@brief Performs a static_cast on the managed pointer, replacing the deleter,
  /// then returns a Counted<> typed to the new type U.
  template <class U, class DU = std::default_delete<U>>
  Counted<U> StaticCast(DU d = DU())
  {
    return Counted<U>(static_cast<U*>(Get()), d);
  }

  ///@return Const reference to the currently used deleter.
  Deleter const& GetDeleter() const
  {
    return m_d;
  }

  ///@return Reference to the currently used deleter.
  Deleter& GetDeleter()
  {
    return m_d;
  }

  // operator overloads
  Counted<T>& operator=(Counted<T> const& rhs)
  {
    Counted<T>(rhs).Swap(*this);
    return *this;
  }

  T& operator*() const
  {
    return *static_cast<T*>(m_p);
  }

  T* operator->() const
  {
    return static_cast<T*>(m_p);
  }

private:
  Deleter m_d;
};

namespace detail
{

template <class T, class U>
using CountedEqEnable = std::enable_if_t<std::is_base_of_v<T, U> || std::is_base_of_v<U, T>>;

}

//==============================================================================
template <class T>
inline
bool operator==(Counted<T> const& p0, std::nullptr_t p1)
{
  return p0.Get() == p1;
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
inline
bool operator==(Counted<T> const& p0, U* p1)
{
  return p0.Get() == p1;
}

//==============================================================================
template <class T>
inline
bool operator==(std::nullptr_t p0, Counted<T> const& p1)
{
  return operator==(p1, p0);
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
inline
bool operator==(T* p0, Counted<U> const& p1)
{
  return operator==(p1, p0);
}

//==============================================================================
template <class T>
inline
bool operator!=(Counted<T> const& p0, std::nullptr_t p1)
{
  return !operator==(p0, p1);
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
inline
bool operator!=(Counted<T> const& p0, U* p1)
{
  return !operator==(p0, p1);
}

//==============================================================================
template <class T>
inline
bool operator!=(std::nullptr_t p0, Counted<T> const& p1)
{
  return !operator==(p1, p0);
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
inline
bool operator!=(T* p0, Counted<U> const& p1)
{
  return !operator==(p1, p0);
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
bool operator==(Counted<T> const& lhs, Counted<U> const& rhs)
{
  return lhs.Get() == rhs.Get();
}

//==============================================================================
template <class T, class U, detail::CountedEqEnable<T, U>* = nullptr>
bool operator!=(Counted<T> const& lhs, Counted<U> const& rhs)
{
  return !operator==(lhs, rhs);
}

} // xr

#endif
