#ifndef XR_CALLBACK_HPP
#define XR_CALLBACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/typeutils.hpp"

namespace xr
{

//==============================================================================
///@brief Base class for a generic callback, which can be a member- or a free-
/// standing function. Carries a pointer to the object (for a member function)
/// or user data (for a free-standing function).
///@note Does not take ownership of the data.
struct CallbackBase
{
  // structors
  explicit CallbackBase(void* data)
  : m_data{ data }
  {}

  virtual ~CallbackBase()
  {}

  // virtual
  virtual CallbackBase* Clone() const =0;

  // operators
  bool operator==(CallbackBase const& other) const
  {
    return m_data == other.m_data && GetTypeId() == other.GetTypeId() &&
      EqualsInternal(other);
  }

  bool operator!=(CallbackBase const& other) const
  {
    return !operator==(other);
  }

protected:
  // data
  void* m_data; // no ownership

  // virtual
  virtual size_t GetTypeId() const = 0;
  virtual bool EqualsInternal(CallbackBase const& other) const =0;

  template <typename T>
  T& GetObject() const
  {
    return *static_cast<T*>(const_cast<void*>(m_data));
  }
};

//==============================================================================
///@brief Callback interface to call a function with the given Return value
/// type and arguments Args.
template <typename Return, typename... Args>
struct Callback: CallbackBase
{
  explicit Callback(void* data)
  : CallbackBase{ data }
  {}

  virtual ~Callback()
  {}

  virtual Return Call(Args...) =0;
};

//==============================================================================
///@brief Concrete generic callback templates and specialisations, catering for
/// member functions, having / not having return values, taking / not taking
/// arguments.
template <class T, typename Return, typename... Args>
struct MemberCallback : Callback<Return, Args...>
{
  using BaseType = Callback<Return, Args...>;
  using SelfType = MemberCallback<T, Return, Args...>;
  using Function = Return(T::*)(Args...);

  MemberCallback(T& object, Function fn)
  : BaseType{ &object },
    m_function{ fn }
  {}

  SelfType* Clone() const override
  {
    return new SelfType(CallbackBase::GetObject<T>(), m_function);
  }

  Return Call(Args... args) override
  {
    return (static_cast<T*>(CallbackBase::m_data)->*m_function)(args...);
  }

protected:
  Function m_function;

  size_t GetTypeId() const override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const override
  {
    return m_function == static_cast<SelfType const&>(other).m_function;
  }
};

template <class T, typename... Args>
struct MemberCallback<T, void, Args...> : Callback<void, Args...>
{
  using BaseType = Callback<void, Args...>;
  using SelfType = MemberCallback<T, void, Args...>;
  using Function = void(T::*)(Args...);

  MemberCallback(T& object, Function fn)
  : BaseType{ &object },
    m_function{ fn }
  {}

  SelfType* Clone() const override
  {
    return new SelfType(CallbackBase::GetObject<T>(), m_function);
  }

  void Call(Args... args) override
  {
    (static_cast<T*>(CallbackBase::m_data)->*m_function)(args...);
  }

protected:
  Function m_function;

  size_t GetTypeId() const override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const override
  {
    return m_function == static_cast<SelfType const&>(other).m_function;
  }
};

//==============================================================================
///@brief Concrete generic callback template and specialisations, catering for
/// free-standing (non-member) functions, having / not having return values,
/// taking / not taking arguments.
template <typename Return, typename... Args>
struct FunctionPtrCallback : Callback<Return, Args...>
{
  using BaseType = Callback<Return, Args...>;
  using SelfType = FunctionPtrCallback<Return, Args...>;
  using Function = Return(*)(Args..., void*);

  FunctionPtrCallback(Function fn, void* userData)
  : BaseType{ userData },
    m_function{ fn }
  {}

  SelfType* Clone() const override
  {
    return new SelfType(m_function, const_cast<void*>(CallbackBase::m_data));
  }

  Return Call(Args... args) override
  {
    return m_function(args..., CallbackBase::m_data);
  }

protected:
  Function m_function;

  size_t GetTypeId() const override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const override
  {
    return m_function == static_cast<SelfType const&>(other).m_function;
  }
};

template <typename... Args>
struct FunctionPtrCallback<void, Args...> : Callback<void, Args...>
{
  using BaseType = Callback<void, Args...>;
  using SelfType = FunctionPtrCallback<void, Args...>;
  using Function = void(*)(Args..., void*);

  FunctionPtrCallback(Function fn, void* userData)
  : BaseType{ userData },
    m_function{ fn }
  {}

  SelfType* Clone() const override
  {
    return new SelfType(m_function, const_cast<void*>(CallbackBase::m_data));
  }

  void Call(Args... args) override
  {
    m_function(args..., CallbackBase::m_data);
  }

protected:
  Function m_function;

  size_t GetTypeId() const override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const override
  {
    return m_function == static_cast<SelfType const&>(other).m_function;
  }
};
//==============================================================================
template <class T, typename Return, typename... Args>
MemberCallback<T, Return, Args...> MakeCallback(T& object,
  Return(T::*fn)(Args...))
{
  return MemberCallback<T, Return, Args...>(object, fn);
}

} //xr

#endif //XR_CALLBACK_HPP
