/* Copyright (C) 2023 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FSM_H
#define FSM_H

#include <limits>
#include <set>
#include <vector>


constexpr unsigned int FSM_INVALID_STATE{std::numeric_limits<unsigned int>::max()};

class CFsmEvent;
class CFsmTransition;
class CFsm;

using Action = bool(void* pContext, CFsmEvent* pEvent);

struct CallbackFunction
{
	Action* pFunction{nullptr};
	void* pContext{nullptr};
};

using StateSet = std::set<unsigned int>;
using TransitionList = std::vector<CFsmTransition*>;

/**
 * Represents a signal in the state machine that a change has occurred.
 * The CFsmEvent objects are under the control of CFsm so
 * they are created and deleted via CFsm.
 */
class CFsmEvent
{
	NONCOPYABLE(CFsmEvent);
public:

	CFsmEvent(unsigned int type, void* pParam);
	~CFsmEvent();

	unsigned int GetType() const
	{
		return m_Type;
	}

	void* GetParamRef()
	{
		return m_Param;
	}

private:
	unsigned int m_Type; // Event type
	void* m_Param; // Event paramater
};


/**
 * An association of event, action and next state.
 */
class CFsmTransition
{
	NONCOPYABLE(CFsmTransition);
public:
	/**
	 * @param action Object executed upon transition.
	 */
	CFsmTransition(const unsigned int state, const CallbackFunction action);

	/**
	 * Set event for which transition will occur.
	 */
	void SetEventType(const unsigned int eventType);
	unsigned int GetEventType() const
	{
		return m_EventType;
	}

	/**
	 * Set next state the transition will switch the system to.
	 */
	void SetNextState(unsigned int nextState);
	unsigned int GetNextState() const
	{
		return m_NextState;
	}

	unsigned int GetCurrState() const
	{
		return m_CurrState;
	}

	CallbackFunction GetAction() const
	{
		return m_Action;
	}

	/**
	 * Executes action for the transition.
	 * @note If there are no action, assume true.
	 * @return whether the action returned true.
	 */
	bool RunAction(CFsmEvent& event) const;

private:
	unsigned int m_CurrState;
	unsigned int m_NextState;
	unsigned int m_EventType;
	CallbackFunction m_Action;
};

/**
 * Manages states, events, actions and transitions
 * between states. It provides an interface for advertising
 * events and track the current state. The implementation is
 * a Mealy state machine, so the system respond to events
 * and execute some action.
 *
 * A Mealy state machine has behaviour associated with state
 * transitions; Mealy machines are event driven where an
 * event triggers a state transition.
 */
class CFsm
{
	NONCOPYABLE(CFsm);
public:

	CFsm();
	virtual ~CFsm();

	/**
	 * Constructs the state machine. This method must be overriden so that
	 * connections are constructed for the particular state machine implemented.
	 */
	virtual void Setup();

	/**
	 * Clear event, transition lists and reset state machine.
	 */
	void Shutdown();

	/**
	 * Adds the specified state to the internal list of states.
	 * @note If a state with the specified ID exists, the state is not added.
	 */
	void AddState(unsigned int state);

	/**
	 * Adds a new transistion to the state machine.
	 * @return a pointer to the new transition.
	 */
	CFsmTransition* AddTransition(unsigned int state, unsigned int eventType, unsigned int nextState,
		Action* pAction = nullptr, void* pContext = nullptr);

	/**
	 * Looks up the transition given the state, event and next state to transition to.
	 */
	CFsmTransition* GetTransition(unsigned int state, unsigned int eventType) const;
	CFsmTransition* GetEventTransition (unsigned int eventType) const;

	/**
	 * Sets the initial state for FSM.
	 */
	void SetFirstState(unsigned int firstState);

	/**
	 * Sets the current state and update the last state to the current state.
	 */
	void SetCurrState(unsigned int state);
	unsigned int GetCurrState() const
	{
		return m_CurrState;
	}

	void SetNextState(unsigned int nextState)
	{
		m_NextState = nextState;
	}

	unsigned int GetNextState() const
	{
		return m_NextState;
	}

	const StateSet& GetStates() const
	{
		return m_States;
	}

	const TransitionList& GetTransitions() const
	{
		return m_Transitions;
	}

	/**
	 * Updates the FSM and retrieves next state.
	 * @return whether the state was changed.
	 */
	bool Update(unsigned int eventType, void* pEventData);

	/**
	 * Verifies whether the specified state is managed by the FSM.
	 */
	bool IsValidState(unsigned int state) const;

	/**
	 * Tests whether the state machine has finished its work.
	 * @note This is state machine specific.
	 */
	virtual bool IsDone() const;

private:
	/**
	 * Verifies whether state machine has already been updated.
	 */
	bool IsFirstTime() const;

	bool m_Done;
	unsigned int m_FirstState;
	unsigned int m_CurrState;
	unsigned int m_NextState;
	StateSet m_States;
	TransitionList m_Transitions;
};

#endif // FSM_H
