#pragma once

#include <map>
#include <set>
#include <string>

class MealyMachine;

class MooreMachine
{
public:
	using State = std::string;
	using MooreTransitions = std::map<std::pair<State, std::string>, State>;
	using MooreOutputs = std::map<State, std::string>;

	MooreMachine() = default;
	explicit MooreMachine(State  initState);
	explicit MooreMachine(const MealyMachine& mealyMachine);

	static MooreMachine FromDotFile(const std::string& name);
	std::string ToDotString() const;
	[[nodiscard]] std::string Print() const;

	[[nodiscard]] std::set<State> GetStates() const;
	[[nodiscard]] State GetStartState() const;
	[[nodiscard]] MooreOutputs GetOutputs() const;
	[[nodiscard]] MooreTransitions GetTransitions() const;
	void AddState(const State& state, const std::string& output);
	void SetStartState(const State& state);
	void SetTransition(const State& fromState, const std::string& input, const State& toState);
	void SetStateOutput(const State& state, const std::string& output);

private:
	std::set<State> m_states;
	MooreTransitions m_transitions;
	MooreOutputs m_outputs;
	State m_startState;
};
