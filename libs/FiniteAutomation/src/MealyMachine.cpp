#include "MealyMachine.h"
#include "MooreMachine.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
const std::regex STATE_REGEX(R"lit(^\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*$)lit");
const std::regex TRANSITION_REGEX(R"lit(^\s*(\w+)\s*->\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*$)lit");
const std::regex LABEL_REGEX("^([^/]+)/(.+)$");

constexpr size_t STATE_WIDTH = 12;
constexpr size_t CELL_WIDTH = 12;

using State = MealyMachine::State;
using TransitionLabel = std::pair<std::string, std::string>;

TransitionLabel ParseLabel(const std::string& label)
{
	std::smatch match;
	if (!std::regex_match(label, match, LABEL_REGEX))
	{
		throw std::runtime_error("Invalid transition label format: " + label);
	}

	return {match[1], match[2]};
}

bool ParseState(const std::string& line, MealyMachine& machine, std::map<std::string, State>& stateMap)
{
	std::smatch match;
	if (!std::regex_match(line, match, STATE_REGEX))
	{
		return false;
	}

	const std::string stateName = match[1];
	stateMap[stateName] = stateName;
	machine.AddState(stateName);

	if (machine.GetStates().size() == 1)
	{
		machine.SetStartState(stateName);
	}

	return true;
}

bool ParseTransition(const std::string& line, MealyMachine& machine, std::map<std::string, State>& stateMap)
{
	std::smatch match;
	if (!std::regex_match(line, match, TRANSITION_REGEX))
	{
		return false;
	}

	const std::string srcStateName = match[1];
	const std::string dstStateName = match[2];
	const std::string label = match[3];

	auto [input, output] = ParseLabel(label);

	const State& srcState = stateMap.at(srcStateName);
	const State& dstState = stateMap.at(dstStateName);

	machine.SetTransition(srcState, input, dstState, output);

	return true;
}

void ParseDot(MealyMachine& machine, std::istream& input)
{
	std::map<std::string, MealyMachine::State> stateMap;
	std::string line;

	while (std::getline(input, line))
	{
		if (line.empty())
		{
			continue;
		}

		if (ParseState(line, machine, stateMap))
		{
			continue;
		}

		if (ParseTransition(line, machine, stateMap))
		{
			continue;
		}
	}
}
} // namespace

MealyMachine::MealyMachine(State initState)
	: m_startState(std::move(initState))
{
}

MealyMachine::MealyMachine(const MooreMachine& mooreMachine)
{
	m_states = mooreMachine.GetStates();
	m_startState = mooreMachine.GetStartState();

	const auto mooreTransitions = mooreMachine.GetTransitions();
	const auto mooreOutputs = mooreMachine.GetOutputs();

	for (const auto& transition : mooreTransitions)
	{
		const State& fromState = transition.first.first;
		const std::string& input = transition.first.second;
		const State& toState = transition.second;
		const std::string& output = mooreOutputs.at(toState);

		SetTransition(fromState, input, toState, output);
	}
}

MealyMachine MealyMachine::FromDotFile(const std::string& name)
{
	std::ifstream file(name);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + name);
	}

	MealyMachine machine;
	ParseDot(machine, file);

	return machine;
}

std::string MealyMachine::ToDotString() const
{
	std::ostringstream oss;
	oss << "digraph mealyMachine {" << std::endl;

	for (const auto& state : m_states)
	{
		oss << state << " [label = \"" << state << "\"]" << std::endl;
	}
	oss << std::endl;

	std::vector<std::tuple<State, State, std::string, std::string>> sortedTransitions;
	for (const auto& trans : m_transitions)
	{
		sortedTransitions.emplace_back(trans.first.first, trans.second.first, trans.first.second, trans.second.second);
	}
	std::ranges::sort(sortedTransitions);

	for (const auto& transition : sortedTransitions)
	{
		oss << std::get<0>(transition) << " -> " << std::get<1>(transition) << " [label = \"" << std::get<2>(transition) << "/" << std::get<3>(transition) << "\"]" << std::endl;
	}

	oss << std::endl;
	return oss.str();
}

std::string MealyMachine::Print() const
{
	if (m_states.empty())
	{
		return "Mealy Machine is empty";
	}

	std::ostringstream oss;

	std::set<std::string> inputs;
	for (const auto& transition : m_transitions)
	{
		inputs.insert(transition.first.second);
	}

	std::vector<State> states(m_states.begin(), m_states.end());
	std::ranges::sort(states);

	oss << "Mealy machine table" << std::endl;
	oss << "Start state: " << m_startState << std::endl;

	oss << std::setw(STATE_WIDTH) << std::left << "Input/State";
	for (const auto& state : states)
	{
		oss << std::setw(CELL_WIDTH) << std::left << state;
	}
	oss << std::endl;

	oss << std::string(STATE_WIDTH, '-');
	for (size_t i = 0; i < states.size(); ++i)
	{
		oss << std::string(CELL_WIDTH, '-');
	}
	oss << std::endl;

	for (const auto& input : inputs)
	{
		oss << std::setw(STATE_WIDTH) << std::left << input;

		for (const auto& state : states)
		{
			auto key = std::make_pair(state, input);
			auto it = m_transitions.find(key);

			if (it != m_transitions.end())
			{
				std::string transition = it->second.first + "/" + it->second.second;
				oss << std::setw(CELL_WIDTH) << std::left << transition;
			}
			else
			{
				oss << std::setw(CELL_WIDTH) << std::left << "-";
			}
		}
		oss << std::endl;
	}

	return oss.str();
}

std::set<State> MealyMachine::GetStates() const
{
	return m_states;
}

State MealyMachine::GetStartState() const
{
	return m_startState;
}

MealyMachine::MealyTransitions MealyMachine::GetTransitions() const
{
	return m_transitions;
}

void MealyMachine::AddState(const State& state)
{
	m_states.insert(state);
}

void MealyMachine::SetStartState(const State& state)
{
	if (m_states.contains(state))
	{
		m_startState = state;
	}
	else
	{
		throw std::invalid_argument("State " + state + " is not in the machine");
	}
}

void MealyMachine::SetTransition(const State& fromState, const std::string& input, const State& toState, const std::string& output)
{
	m_states.insert(fromState);
	m_states.insert(toState);

	m_transitions[{fromState, input}] = {toState, output};
}
