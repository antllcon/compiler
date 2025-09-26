#pragma once

#include <map>
#include <set>
#include <string>

class MooreMachine;

class MealyMachine
{
public:
	using State = std::string;
	using MealyTransitions = std::map<std::pair<State, std::string>, std::pair<State, std::string>>;

	MealyMachine() = default;
	explicit MealyMachine(State  initState);
	explicit MealyMachine(const MooreMachine& mooreMachine);

	static MealyMachine FromDotFile(const std::string& name);
	std::string ToDotString() const;
	[[nodiscard]] std::string Print() const;

	[[nodiscard]] std::set<State> GetStates() const;
	[[nodiscard]] State GetStartState() const;
	[[nodiscard]] MealyTransitions GetTransitions() const;
	void AddState(const State& state);
	void SetStartState(const State& state);
	void SetTransition(const State& fromState, const std::string& input, const State& toState, const std::string& output);

private:
	std::set<State> m_states;
	MealyTransitions m_transitions;
	State m_startState;
};
