#ifndef XR_SCOPEGUARD_HPP
#define XR_SCOPEGUARD_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

namespace  xr
{
namespace
{
//==============================================================================
///@brief Scope Guard provides a lightweight, RAII based facility to execute
/// cleanup / undo code upon scope exit (the destruction of the scope guard) -
/// whichever way that might happen. Once this effect is no longer required,
/// the Scope Guard can be Release()d.
///@note It is recommended to only create Scope Guards as function locals and
/// on the stack.
class  ScopeGuardCore
{
public:
  // structors
  ScopeGuardCore()
  :  m_isEngaged(true)
  {}

  ScopeGuardCore(const ScopeGuardCore& rhs)
  :  m_isEngaged(rhs.m_isEngaged)
  {
    rhs.Release();
  }

  // general
  void  Release() const
  {
    m_isEngaged = false;
  }

  // disabled
  ScopeGuardCore&  operator=(const ScopeGuardCore& other) = delete;

protected:
  // internal
  bool  IsEngaged() const { return m_isEngaged; }

private:
  // data
  mutable bool  m_isEngaged;
};

}

typedef const ScopeGuardCore&  ScopeGuard;

//==============================================================================
template  <class Func>
class  ScopeGuardImpl:  public ScopeGuardCore
{
public:
  // structors
  explicit ScopeGuardImpl(const Func& f)
  : ScopeGuardCore(),
    m_func(f)
  {}

  ~ScopeGuardImpl()
  {
    if(IsEngaged()) try
    {
      m_func();
    }
    catch(...)
    {
      // We cannot guarantee that m_func doesn't throw;
      // If it does - during stack unwinding due to a previous exception -,
      // then we cannot do much; we move on. To minimize the chances of
      // this happening, always strive to guard the operation that's least
      // likely to throw.
    }
  }

private:
  // data
  Func  m_func;
};

//==============================================================================
///@brief Facilitates the creation of a ScopeGuard via template deduction.
template  <typename Func>
ScopeGuardImpl<Func>  MakeScopeGuard(Func f)
{
  return ScopeGuardImpl<Func>(f);
}

}  // xr

#endif  //XR_SCOPEGUARD_HPP
