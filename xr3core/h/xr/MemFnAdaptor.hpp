#ifndef XR_MEMFNADAPTOR_HPP
#define XR_MEMFNADAPTOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

namespace	xr
{

//==============================================================================
///@brief Adapts the member function that its template is instantiated with to
/// a non-member function whose first argument is the object on which the
/// member function is to be called.
template	<typename Return, class Callee,
	typename Arg1 = void,
	typename Arg2 = void,
	typename Arg3 = void,
	typename Arg4 = void,
	typename Arg5 = void,
	typename Arg6 = void>
struct	MemFnAdaptor;

// specialisation for 0 arguments
template	<typename Return, class Callee>
struct	MemFnAdaptor<Return, Callee>
{
	template	<Return(Callee::*Method)()>
	static Return	Function(Callee& c)
	{
		return (c.*Method)();
	}
};

template	<typename Return, class Callee>
struct	MemFnAdaptor<Return, const Callee>
{
	template	<Return(Callee::*Method)() const>
	static Return	Function(const Callee& c)
	{
		return (c.*Method)();
	}
};

// specialisation for 1 argument
template	<typename Return, class Callee,
	typename Arg1>
struct	MemFnAdaptor<Return, Callee, Arg1>
{
	template	<Return(Callee::*Method)(Arg1)>
	static Return	Function(Callee& c, Arg1 arg1)
	{
		return (c.*Method)(arg1);
	}
};

template	<typename Return, class Callee,
	typename Arg1>
struct	MemFnAdaptor<Return, const Callee, Arg1>
{
	template	<Return(Callee::*Method)(Arg1) const>
	static Return	Function(const Callee& c, Arg1 arg1)
	{
		return (c.*Method)(arg1);
	}
};

// specialisation for 2 arguments
template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2>
struct	MemFnAdaptor<Return, Callee, Arg1, Arg2>
{
	template	<Return(Callee::*Method)(Arg1, Arg2)>
	static Return	Function(Callee& c, Arg1 arg1, Arg2 arg2)
	{
		return (c.*Method)(arg1, arg2);
	}
};

template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2>
struct	MemFnAdaptor<Return, const Callee, Arg1, Arg2>
{
	template	<Return(Callee::*Method)(Arg1, Arg2) const>
	static Return	Function(const Callee& c, Arg1 arg1, Arg2 arg2)
	{
		return (c.*Method)(arg1, arg2);
	}
};

// specialisation for 3 arguments
template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3>
struct	MemFnAdaptor<Return, Callee, Arg1, Arg2, Arg3>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3)>
	static Return	Function(Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		return (c.*Method)(arg1, arg2, arg3);
	}
};

template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3>
struct	MemFnAdaptor<Return, const Callee, Arg1, Arg2, Arg3>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3) const>
	static Return	Function(const Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		return (c.*Method)(arg1, arg2, arg3);
	}
};

// specialisation for 4 arguments
template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4>
struct	MemFnAdaptor<Return, Callee, Arg1, Arg2, Arg3, Arg4>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4)>
	static Return	Function(Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4);
	}
};

template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4>
struct	MemFnAdaptor<Return, const Callee, Arg1, Arg2, Arg3, Arg4>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4) const>
	static Return	Function(const Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4);
	}
};

// specialisation for 5 arguments
template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4,
	typename Arg5>
struct	MemFnAdaptor<Return, Callee, Arg1, Arg2, Arg3, Arg4, Arg5>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4, Arg5)>
	static Return	Function(Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4, arg5);
	}
};

template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4,
	typename Arg5>
struct	MemFnAdaptor<Return, const Callee, Arg1, Arg2, Arg3, Arg4, Arg5>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4, Arg5) const>
	static Return	Function(const Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4, arg5);
	}
};
// specialisation for 6 arguments
template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4,
	typename Arg5,
	typename Arg6>
struct	MemFnAdaptor
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>
	static Return	Function(Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4, arg5, arg6);
	}
};

template	<typename Return, class Callee,
	typename Arg1,
	typename Arg2,
	typename Arg3,
	typename Arg4,
	typename Arg5,
	typename Arg6>
struct	MemFnAdaptor<Return, const Callee, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>
{
	template	<Return(Callee::*Method)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>
	static Return	Function(const Callee& c, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	{
		return (c.*Method)(arg1, arg2, arg3, arg4, arg5, arg6);
	}
};

}	// xr

#endif //XR_MEMFNADAPTOR_HPP
