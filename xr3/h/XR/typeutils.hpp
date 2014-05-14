//
// Nuclear Heart Games
// XRhodes
//
// types.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    16/07/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TYPESUTILS_HPP
#define XR_TYPESUTILS_HPP

namespace XR
{

//==============================================================================
///@brief Used in type operations, this type indicates that the result is not
/// meaningful and should not be used.
class Nontype
{};

//==============================================================================
///@brief Ternary typedef operator. Based on the value of @a kCheck,
/// propagates the definition of @a A (if true) or @a B (if false) as @a Type.
template  <bool kCheck, typename A, typename B>
struct  TypeSelect
{
  typedef A Type;
};

template  <typename A, typename B>
struct  TypeSelect<false, A, B>
{
  typedef B Type;
};

//==============================================================================
///@brief Tells you whether T is a constant type. If it is, it defines the
/// corresponding non-const type in UnconstType.
template  <typename T>
struct  IsConst
{
  enum { kResult = false };

  typedef Nontype UnconstType;
};

template  <typename T>
struct  IsConst<const T>
{
  enum { kResult = true };

  typedef T UnconstType;
};

//==============================================================================
///@brief Tells you whether T is a pointer type. If it is, it defines the
/// corresponding pointed type in PointedType.
template  <typename T>
struct  IsPointer
{
  enum { kResult = false };

  typedef Nontype  PointedType;
};

template  <typename T>
struct  IsPointer<T*>
{
  enum { kResult = true };

  typedef T PointedType;
};

//==============================================================================
///@brief Enforces T to be derived from B via implicit upcasting.
template  <class T, class B>
class AssertBase
{
  static void Constraint(T* p)
  {
    B* pBase = p;
  }
 
public:
  AssertBase()
  {
    void(*p)(T*) = Constraint;
  }
};

//==============================================================================
///@brief Static typeId based on of address of function-static variable.
///@note  TypeId() doesn't persist across executions/platforms.
///@note  Remember, TypeId for const T or T* != TypeId for T.
template  <typename T>
inline
unsigned int TypeId()
{
  static char c;
  return reinterpret_cast<unsigned int>(&c);
}

}  // XR

#endif  //XR_TYPESUTILS_HPP
