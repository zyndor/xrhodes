//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_COMMAND_HPP
#define XR_COMMAND_HPP

namespace	XR
{

//==============================================================================
template	<typename Return>
class	Command
{
public:
	// structors
	Command()	{}
	virtual ~Command()	{}

	// virtual
	virtual Return	Execute() =0;
};

//==============================================================================
///@brief A Command that calls a function with up to 8 arguments.
template	<typename	Return,
	typename Arg1 = void,
	typename Arg2 = void,
	typename Arg3 = void,
	typename Arg4 = void,
	typename Arg5 = void,
	typename Arg6 = void,
	typename Arg7 = void,
	typename Arg8 = void>
class	FnCommand;

// specialisation - 0 arguments
template	<typename Return>
class	FnCommand<Return>:	public Command<Return>
{
public:
	// types
	typedef Return	ReturnType;

	typedef	Return(*Function)();

	// structors
	explicit FnCommand(Function fn)
	:	Command<Return>(),
		m_pFunction(fn)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<Function>(m_pFunction))();
	}

protected:
	// data
	void*	m_pFunction;
};

// specialisation - 1 argument
template	<typename Return, typename Arg1>
class	FnCommand<Return, Arg1>:
	public FnCommand<Return>
{
public:
	// types
	typedef FnCommand<Return>	BaseType;

	typedef	Return(*Function)(Arg1);

	// structors
	FnCommand(Function fn, Arg1 arg1)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn)),
		m_arg1(arg1)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))(m_arg1);
	}

protected:
	// data
	Arg1	m_arg1;
};

// specialisation - 2 arguments
template	<typename Return, typename Arg1, typename Arg2>
class	FnCommand<Return, Arg1, Arg2>:
	public FnCommand<Return, Arg1>
{
public:
	// types
	typedef FnCommand<Return, Arg1>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1),
		m_arg2(arg2)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, m_arg2);
	}

protected:
	// data
	Arg2	m_arg2;
};

// specialisation - 3 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3>
class	FnCommand<Return, Arg1, Arg2, Arg3>:
	public FnCommand<Return, Arg1, Arg2>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2),
		m_arg3(arg3)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, m_arg3);
	}

protected:
	// data
	Arg3	m_arg3;
};

// specialisation - 4 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class	FnCommand<Return, Arg1, Arg2, Arg3, Arg4>:
	public FnCommand<Return, Arg1, Arg2, Arg3>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2, Arg3>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3, Arg4);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2, arg3),
		m_arg4(arg4)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, BaseType::m_arg3, m_arg4);
	}

protected:
	// data
	Arg4	m_arg4;
};

// specialisation - 5 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class	FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5>:
	public FnCommand<Return, Arg1, Arg2, Arg3, Arg4>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2, Arg3, Arg4>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3, Arg4, Arg5);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2, arg3, arg4),
		m_arg5(arg5)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, BaseType::m_arg3, BaseType::m_arg4, m_arg5);
	}

protected:
	// data
	Arg5	m_arg5;
};

// specialisation - 6 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
class	FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>:
	public FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2, arg3, arg4, arg5),
		m_arg6(arg6)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, BaseType::m_arg3, BaseType::m_arg4,
				BaseType::m_arg5, m_arg6);
	}

protected:
	// data
	Arg6	m_arg6;
};

// specialisation - 7 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
class	FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>:
	public FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2, arg3, arg4, arg5, arg6),
		m_arg7(arg7)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, BaseType::m_arg3, BaseType::m_arg4,
				BaseType::m_arg5, BaseType::m_arg6, m_arg7);
	}

protected:
	// data
	Arg7	m_arg7;
};

// specialisation - 8 arguments
template	<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
class	FnCommand:
	public FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>
{
public:
	// types
	typedef FnCommand<Return, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>	BaseType;

	typedef	Return(*Function)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8);

	// structors
	FnCommand(Function fn, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8)
	:	BaseType(reinterpret_cast<typename BaseType::Function>(fn), arg1, arg2, arg3, arg4, arg5, arg6, arg7),
		m_arg8(arg8)
	{}

	// general
	virtual Return	Execute()
	{
		return (*reinterpret_cast<typename BaseType::Function>(BaseType::m_pFunction))
			(BaseType::m_arg1, BaseType::m_arg2, BaseType::m_arg3, BaseType::m_arg4,
				BaseType::m_arg5, BaseType::m_arg6, BaseType::m_arg7, m_arg8);
	}

protected:
	// data
	Arg8	m_arg8;
};

}	// XR

#endif	//XR_COMMAND_HPP