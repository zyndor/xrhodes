#ifndef XR_STATE_HPP
#define XR_STATE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <list>
#include <cstdint>

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
		void	Push(State& s);	// no ownership transfer
		void	Change(State& s);	// no ownership transfer
		void	Pop();

		void	Update(int32_t tDelta);
		void	Render();

		void	Clear();

	protected:
		// types
		typedef std::list<State*>	Stack;
		
		// data
		Stack	m_states;
		
		// internal
		void	_Push(State& s);
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
	virtual void	Update(int32_t ms, Result& result) =0;
	virtual void	Render() =0;
	virtual void	Exit() =0;
	virtual void	Shutdown() {}
};

} // XR

#endif // XR_STATE_HPP
