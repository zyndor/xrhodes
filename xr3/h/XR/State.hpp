//
// Nuclear Heart Games
// XRhodes
//
// State.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    29/11/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_STATE_HPP
#define XR_STATE_HPP

#include <list>

namespace XR
{

//==============================================================================
///@brief An application state. Life cycle:
/// Init() when pushed onto the stack,
///   Enter() when becoming the current state,
///     Update(), Render() while active
///   Exit() when a new state was requested,
/// Shutdown() when removed from the stack.
//==============================================================================
class State
{
public:
	// types
	//==============================================================================
	///@brief Maintains a stack of states and manages their life cycles.
	//==============================================================================
	class	Manager
	{
	public:
		// structors
		Manager();
		~Manager();

		// general
		void	Push(State* p);	// no ownership transfer
		void	Change(State* p);	// no ownership transfer
		void	Pop();

		void	Update(int32 tDelta);
		void	Render();

		void	Clear();

	protected:
		// types
		typedef std::list<State*>	Stack;
		
		// data
		Stack	m_states;
		
		// internal
		void	_Push(State* p);
		void	_Pop();
	};

	//==============================================================================
	///@brief Signifies the result of a State Update() encapsulating State::Manager
	/// operation and a next State to go to.
	//==============================================================================
	struct	Result
	{
		// types
		enum	Operation
		{
			NONE,
			PUSH,
			CHANGE,
			POP
		};

		// structors
		Result()
		:	operation(NONE),
			pState(0)
		{}

		Result(Operation op, State* p = 0)
		:	operation(op),
			pState(p)
		{}

		// data
		Operation	operation;
		State*		pState;
	};
	
	// structors
	State();
	virtual ~State();
	
	// general
	virtual void	Init()	{}
	virtual void	Enter() =0;
	virtual void	Update(int32 ms, Result& result) =0;
	virtual void	Render() =0;
	virtual void	Exit() =0;
	virtual void	Shutdown() {}
};

} // XR

#endif // XR_STATE_HPP
