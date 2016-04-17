//
// Nuclear Heart Games
// XRhodes
//
// CrossObject.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/09/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
// 
//==============================================================================
#ifndef XR_CROSSOBJECT_HPP
#define XR_CROSSOBJECT_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
///@brief pImpl wrapper class with basic accessors. CrossObject does not know
/// about the ownership of the object it's pointing to, nor about its type; you
/// will have to.
class CrossObject
{
protected:  
  // data
  void* m_pImpl;  // no ownership

  // structors
  CrossObject();
  CrossObject(void* pImpl);
  ~CrossObject();
  
  // general
  bool        HasImpl() const;
  
  void*       GetImpl();
  const void* GetImpl() const;
  
  ///@brief   Sets @a pImpl as the implementation.
  ///@return  The pointer to the previous implementation. This is your chance to
  ///         catch it.
  void*       SwapImpl(void* pImpl);  // no ownership transfer
};

//==============================================================================
// implementation
//==============================================================================
inline
void* CrossObject::GetImpl()
{
  return m_pImpl;
}

//==============================================================================
inline
const void* CrossObject::GetImpl() const
{
  return m_pImpl;
}

//==============================================================================
inline
bool CrossObject::HasImpl() const
{
  return m_pImpl != 0;
}

//==============================================================================
inline
void* CrossObject::SwapImpl( void* pImpl )
{
  void* pOld(m_pImpl);
  m_pImpl = pImpl;
  return pOld;
}

} // XR

//==============================================================================
#define XR_CROSS_OBJECT_USING() public:  using CrossObject::HasImpl; using CrossObject::GetImpl; using CrossObject::SwapImpl;

//==============================================================================
///@brief Declares the symbols required for CrossObject derivatives.
///@param a     Your CrossObject subclass.
#define XR_CROSS_OBJECT_DECL(a)\
  XR_CROSS_OBJECT_USING()\
public:\
  static void*  NewImpl();\
  static void   DeleteImpl(void* pImpl);\
  \
  a(): CrossObject() {}\
  a(void* pImpl): CrossObject(pImpl) {}\
  ~a() {}\


//==============================================================================
///@brief Defines functions required for CrossObject derivatives.
///@param a     Your CrossObject subclass.
///@param aImpl The implementation class that your CrossObject uses.
///@note  @a aImpl must declare a default constructor, which is going
///       to be used by NewImpl() and CreateManaged().
#define XR_CROSS_OBJECT_DEF(a, aImpl)\
void* a::NewImpl()\
{\
  return new aImpl();\
}\
\
void  a::DeleteImpl(void* pImpl)\
{\
  delete static_cast<aImpl*>(pImpl);\
}\


#endif // XR_CROSSOBJECT_HPP
