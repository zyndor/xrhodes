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
#include <type_traits>
#include <tuple>

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
  explicit CallbackBase(void* data) noexcept
  : m_data{ data }
  {}

  virtual ~CallbackBase() noexcept = default;

  // virtual
  [[nodiscard]] virtual CallbackBase* Clone() const =0;

  // operators
  bool operator==(CallbackBase const& other) const noexcept
  {
    return m_data == other.m_data && GetTypeId() == other.GetTypeId() &&
      EqualsInternal(other);
  }

  bool operator!=(CallbackBase const& other) const noexcept
  {
    return !operator==(other);
  }

protected:
  // data
  void* m_data; // no ownership

  // virtual
  virtual size_t GetTypeId() const noexcept = 0;
  virtual bool EqualsInternal(CallbackBase const& other) const noexcept =0;

  // internal
  template <typename T>
  T& GetObject() const noexcept
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
  // types
  using SelfType = Callback<Return, Args...>;

  // structors
  explicit Callback(void* data) noexcept
  : CallbackBase{ data }
  {}

  // virtual
  [[nodiscard]] virtual SelfType* Clone() const =0;
  virtual Return Call(Args...) const =0;
};

//==============================================================================
///@brief Concrete generic callback templates and specialisations, catering for
/// member functions, having / not having return values, taking / not taking
/// arguments.
template <class T, typename Return, typename... Args>
struct MemberCallback : Callback<Return, Args...>
{
  // types
  using BaseType = Callback<Return, Args...>;
  using SelfType = MemberCallback<T, Return, Args...>;
  using Function = typename std::conditional<std::is_const<T>::value,
    Return(T::*)(Args...) const,
    Return(T::*)(Args...)>::type;

  // structors
  MemberCallback(T& object, Function fn) noexcept
  : BaseType{ &object },
    m_function{ fn }
  {}

  // general
  [[nodiscard]] SelfType* Clone() const override
  {
    return new SelfType(CallbackBase::GetObject<T>(), m_function);
  }

  Return Call(Args... args) const override
  {
    return (static_cast<T*>(CallbackBase::m_data)->*m_function)(args...);
  }

protected:
  // data
  Function m_function;

  // internal
  size_t GetTypeId() const noexcept override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const noexcept override
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
  // types
  using BaseType = Callback<Return, Args...>;
  using SelfType = FunctionPtrCallback<Return, Args...>;
  using Function = Return(*)(Args..., void*);

  // structors
  FunctionPtrCallback(Function fn, void* userData = nullptr) noexcept
  : BaseType{ userData },
    m_function{ fn }
  {}

  // general
  [[nodiscard]] SelfType* Clone() const override
  {
    return new SelfType(m_function, const_cast<void*>(CallbackBase::m_data));
  }

  Return Call(Args... args) const override
  {
    return m_function(args..., CallbackBase::m_data);
  }

protected:
  // data
  Function m_function;

  // internal
  size_t GetTypeId() const noexcept override
  {
    return TypeId<SelfType>();
  };

  bool EqualsInternal(CallbackBase const& other) const noexcept override
  {
    return m_function == static_cast<SelfType const&>(other).m_function;
  }
};

//==============================================================================
///@brief Convenience function to create a MemberCallback from an object and
/// a method of its type.
template <class T, typename Return, typename... Args>
MemberCallback<T, Return, Args...> MakeCallback(T& object, Return(T::*fn)(Args...)) noexcept
{
  return MemberCallback<T, Return, Args...>(object, fn);
}

//==============================================================================
///@brief Convenience function to create a MemberCallback from a const object and
/// a const method of its type.
template <class T, typename Return, typename... Args>
MemberCallback<T, Return, Args...> MakeCallback(T const& object, Return(T::*fn)(Args...) const) noexcept
{
  return MemberCallback<T, Return, Args...>(object, fn);
}

namespace detail
{

template <typename Return, typename Tuple>
struct MakeCallbackImpl;

template <typename Return, typename... Args>
struct MakeCallbackImpl<Return, std::tuple<Args...>>
{
  using Callback = FunctionPtrCallback<Return, Args...>;
};

template <typename First, typename... Rest>
struct RemoveLast
{
  using Type = decltype(std::tuple_cat(
    std::declval<std::tuple<First>>(),
    std::declval<typename RemoveLast<Rest...>::Type>()
  ));
};

template <typename First>
struct RemoveLast<First>
{
  using Type = std::tuple<>;
};

}

//==============================================================================
///@brief Convenience function for creating a FunctionPtrCallback from a free standing
/// function.
template <typename Return, typename... Args>
auto MakeCallback(Return(*fn)(Args...), void* data = nullptr) noexcept
{
  return typename detail::MakeCallbackImpl<
    Return, typename detail::RemoveLast<Args...>::Type>::Callback(fn, data);
}

} //xr

#endif //XR_CALLBACK_HPP
