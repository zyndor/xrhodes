//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_COUNTED_HPP
#define XR_COUNTED_HPP

#include <memory>

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

template <class T, class D = std::default_delete<T>>
class Counted: protected CountedCore
{
public:
  // using
  using Deleter = typename std::remove_reference<D>::type;
  using CountedCore::operator bool;

  // structors
  explicit Counted(T* p = nullptr)
  : CountedCore(p),
    m_d()
  {}

  Counted(Counted<T, D> const& rhs)
  : CountedCore(rhs.m_p),
    m_d(rhs.m_d)
  {}

  Counted(Counted<T, D>&& rhs)
  : CountedCore(nullptr),
    m_d(std::forward<Deleter>(rhs.GetDeleter()))
  {
    std::swap(m_p, rhs.m_p);
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

  template <typename U>
  Counted<U, D> StaticCast()
  {
    return Counted<U, D>(static_cast<U*>(Get()));
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
  Counted<T, D>& operator=(Counted<T, D>& rhs)
  {
    Counted<T, D> temp(rhs);
    std::swap(m_p, temp.m_p);
    return *this;
  }

  T const& operator*() const
  {
    return *static_cast<T*>(m_p);
  }

  T& operator*()
  {
    return *static_cast<T*>(m_p);
  }

  T const* operator->() const
  {
    return static_cast<T*>(m_p);
  }

  T* operator->()
  {
    return static_cast<T*>(m_p);
  }

private:
  Deleter m_d;
};

} // XR

#endif
