//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_COUNTED_HPP
#define XR_COUNTED_HPP

#include <memory>
#include <functional>

namespace XR
{

//==============================================================================
///@brief Interface for implementing intrusive reference counting.
class Countable
{
public:
  virtual ~Countable() {};

  virtual void Acquire() = 0;

  ///@return True if the reference count has reached zero, false otherwise.
  virtual bool Release() = 0;
};

//==============================================================================
///@brief Smart pointer type with intrusive reference counting.
class CountedCore
{
public:
  // operator overloads
  operator bool() const
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

template <class T>
class Counted: protected CountedCore
{
public:
  // using
  using CountedCore::operator bool;
  using Deleter = std::function<void(T*)>;

  // structors
  explicit Counted(Deleter d)
  : Counted(nullptr, d)
  {}

  Counted(T* p = nullptr)
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
  : CountedCore(nullptr)
  {
    Swap(rhs);
  }

  ~Counted()
  {
    Reset(nullptr);
  }

  // general
  T* Get()
  {
    return static_cast<T*>(m_p);
  }

  T const* Get() const
  {
    return static_cast<T const*>(m_p);
  }

  T* Release()
  {
    T* p = static_cast<T*>(m_p);
    m_p = nullptr;
    return p;
  }

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

  template <typename D>
  void Reset(T* p, D d)
  {
    Reset(p);

    m_d = d;
  }

  void Swap(Counted<T>& rhs)
  {
    std::swap(m_p, rhs.m_p);
    std::swap(m_d, rhs.m_d);
  }

  template <typename U, typename DU = std::default_delete<U>>
  Counted<U> StaticCast(DU d = DU())
  {
    return Counted<U>(static_cast<U*>(Get()), d);
  }

  Deleter const& GetDeleter() const
  {
    return m_d;
  }

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

//==============================================================================
template <typename T>
inline
bool operator==(Counted<T> const& p0, std::nullptr_t p1)
{
  return p0.Get() == p1;
}

//==============================================================================
template <typename T>
inline
bool operator==(Counted<T> const& p0, T* p1)
{
  return p0.Get() == p1;
}

//==============================================================================
template <typename T>
inline
bool operator==(std::nullptr_t p0, Counted<T> const& p1)
{
  return operator==(p1, p0);
}

//==============================================================================
template <typename T>
inline
bool operator==(T* p0, Counted<T> const& p1)
{
  return operator==(p1, p0);
}

//==============================================================================
template <typename T>
inline
bool operator!=(Counted<T> const& p0, std::nullptr_t p1)
{
  return !operator==(p0, p1);
}

//==============================================================================
template <typename T>
inline
bool operator!=(Counted<T> const& p0, T* p1)
{
  return !operator==(p0, p1);
}

//==============================================================================
template <typename T>
inline
bool operator!=(std::nullptr_t p0, Counted<T> const& p1)
{
  return !operator==(p1, p0);
}

//==============================================================================
template <typename T>
inline
bool operator!=(T* p0, Counted<T> const& p1)
{
  return !operator==(p1, p0);
}

//==============================================================================
template <typename T>
bool operator==(Counted<T> const& lhs, Counted<T> const& rhs)
{
  return lhs.Get() == rhs.Get();
}

//==============================================================================
template <typename T>
bool operator!=(Counted<T> const& lhs, Counted<T> const& rhs)
{
  return !operator==(lhs, rhs);
}

} // XR

#endif
