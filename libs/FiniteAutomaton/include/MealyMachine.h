#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>

namespace FMachine
{

class MealyMachine
{
public:
	friend class Converter;

	using State = std::string;
	using Input = char;
	using Output = char;

	struct TransitionInfo
	{
		State from;
		Input on;
		State to;
		Output out;
	};

	MealyMachine() = default;
	explicit MealyMachine(const State& initState);

	void SetInitState(const State& state);
	const State& GetInitState() const;
	void AddTransitions(std::initializer_list<TransitionData> transitions);
	void Print(std::ostream& os = std::cout) const;

private:
	State m_initState;
	std::set<State> m_states;
	std::set<Input> m_inputAlphabet;
	std::set<Output> m_outputAlphabet;
	std::map<State, std::map<Input, State>> m_transitions;
	std::map<State, std::map<Input, Output>> m_outputs;
};

} // namespace FMachine