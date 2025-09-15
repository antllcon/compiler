#include "../include/MealyMachine.h"

namespace FMachine
{

MealyMachine::MealyMachine(const State& initState)
	: m_initState(initState)
{
	m_states.insert(initState);
}

void MealyMachine::SetInitState(const State& state)
{
	m_initState = state;
	m_states.insert(state);
}

const MealyMachine::State& MealyMachine::GetInitState() const
{
	return m_initState;
}

void MealyMachine::AddTransitions(std::initializer_list<TransitionData> transitions)
{
	for (const auto& t : transitions)
	{
		m_states.insert(t.from);
		m_states.insert(t.to);
		m_inputAlphabet.insert(t.on);
		m_outputAlphabet.insert(t.out);
		m_transitions[t.from][t.on] = t.to;
		m_outputs[t.from][t.on] = t.out;
	}
}

void MealyMachine::Print(std::ostream& os) const
{
	os << "Mealy Machine\n";
	os << "Initial State: " << m_initState << "\n";
	os << "Transitions (from, input) -> (to, output):\n";
	for (const auto& state : m_states)
	{
		if (m_transitions.contains(state))
		{
			for (const auto& [input, state_to] : m_transitions.at(state))
			{
				const Output output = m_outputs.at(state).at(input);
				os << "  (" << state << ", " << input << ") -> (" << state_to << ", " << output << ")\n";
			}
		}
	}
}

} // namespace FMachine