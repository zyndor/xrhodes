//
// Nuclear Heart Games
// XRhodes
//
// RefHolder.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    20/03/2016
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#if !defined XR_REFHOLDER_HPP
#define XR_REFHOLDER_HPP

namespace	XR
{

//==============================================================================
///@brief	Facilitates passing by reference in environments (templates) which
///       store by value by default.
template	<typename T>
class	RefHolder
{
public:
	// typename
	typedef T	ValueType;

	// structors
	RefHolder(ValueType& v)
	:	m_pObject(&v)
	{}

	// operators
		operator ValueType&() const
	{
		return *m_pObject;
	}

private:
	// data
	ValueType*	m_pObject;	// no ownership
};

//==============================================================================
///@brief	Facilitates the creation of a RefHolder object for a given value.
template	<typename T>
RefHolder<T>	MakeRefHolder(T& t)
{
	return RefHolder<T>(t);
}

}	// XR

#endif	//XR_REFHOLDER_HPP