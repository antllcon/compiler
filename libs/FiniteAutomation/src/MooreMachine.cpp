#include "MooreMachine.h"
#include "MealyMachine.h"

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
const std::regex STATE_LABEL_REGEX("^([^/]+)/(.+)$");
const std::regex TRANSITION_REGEX(R"lit(^\s*(\w+)\s*->\s*(\w+)\s*\[label\s*=\s*"([^"]*)"\]\s*$)lit");

constexpr size_t STATE_WIDTH = 12;
constexpr size_t CELL_WIDTH = 12;

using State = MooreMachine::State;

std::string GetBaseStateName(const std::string& stateName, const std::set<std::string>& allOutputs)
{
	for (const auto& output : allOutputs)
	{
		const std::string suffix = "_" + output;
		if (stateName.size() > suffix.size() && stateName.substr(stateName.size() - suffix.size()) == suffix)
		{
			return stateName.substr(0, stateName.size() - suffix.size());
		}
	}
	return stateName;
}

bool ParseMooreState(const std::string& line, MooreMachine& machine, std::map<std::string, State>& stateMap)
{
	std::smatch match;
	if (!std::regex_match(line, match, STATE_REGEX))
	{
		return false;
	}

	const std::string stateName = match[1];
	const std::string stateLabel = match[2];

	std::smatch labelMatch;
	if (std::regex_match(stateLabel, labelMatch, STATE_LABEL_REGEX))
	{
		const std::string nameInLabel = labelMatch[1];
		const std::string output = labelMatch[2];
		stateMap[stateName] = nameInLabel;
		machine.AddState(nameInLabel, output);
	}
	else
	{
		stateMap[stateName] = stateLabel;
		machine.AddState(stateLabel, "");
	}

	if (machine.GetStates().size() == 1)
	{
		machine.SetStartState(stateMap.at(stateName));
	}

	return true;
}

bool ParseMooreTransition(const std::string& line, MooreMachine& machine, const std::map<std::string, State>& stateMap)
{
	std::smatch match;
	if (!std::regex_match(line, match, TRANSITION_REGEX))
	{
		return false;
	}

	const std::string srcStateName = match[1];
	const std::string dstStateName = match[2];
	const std::string input = match[3];

	const State& srcState = stateMap.at(srcStateName);
	const State& dstState = stateMap.at(dstStateName);

	machine.SetTransition(srcState, input, dstState);
	return true;
}

void ParseDotMoore(MooreMachine& machine, std::istream& input)
{
	std::map<std::string, State> stateMap;
	std::string line;

	while (std::getline(input, line))
	{
		if (line.empty())
		{
			continue;
		}

		if (ParseMooreState(line, machine, stateMap))
		{
			continue;
		}

		if (ParseMooreTransition(line, machine, stateMap))
		{
			continue;
		}
	}
}
} // namespace

MooreMachine::MooreMachine(State initState)
	: m_startState(std::move(initState))
{
}

MooreMachine::MooreMachine(const MealyMachine& mealyMachine)
{
	if (mealyMachine.GetStates().empty())
	{
		return;
	}

	std::set<std::string> allOutputs;
	for (const auto& transition : mealyMachine.GetTransitions())
	{
		allOutputs.insert(transition.second.second);
	}

	std::map<std::pair<State, std::string>, State> mooreStateMap;

	for (const auto& transition : mealyMachine.GetTransitions())
	{
		const auto& toState = transition.second.first;
		const auto& output = transition.second.second;
		std::pair<State, std::string> key = {toState, output};

		if (!mooreStateMap.contains(key))
		{
			State baseName = GetBaseStateName(toState, allOutputs);
			State newMooreStateName = baseName + "_" + output;

			mooreStateMap[key] = newMooreStateName;
			this->AddState(newMooreStateName, output);
		}
	}

	const State& mealyStart = mealyMachine.GetStartState();
	this->AddState(mealyStart, "(L)");
	this->SetStartState(mealyStart);

	for (const auto& mealyTrans : mealyMachine.GetTransitions())
	{
		const auto& fromStateMealy = mealyTrans.first.first;
		const auto& input = mealyTrans.first.second;
		const auto& toStateMealy = mealyTrans.second.first;
		const auto& output = mealyTrans.second.second;

		const State& toStateMoore = mooreStateMap.at({toStateMealy, output});

		for (const auto& pair : mooreStateMap)
		{
			if (GetBaseStateName(pair.first.first, allOutputs) == GetBaseStateName(fromStateMealy, allOutputs))
			{
				const State& fromStateMoore = pair.second;
				this->SetTransition(fromStateMoore, input, toStateMoore);
			}
		}

		if (GetBaseStateName(fromStateMealy, allOutputs) == GetBaseStateName(mealyStart, allOutputs))
		{
			this->SetTransition(mealyStart, input, toStateMoore);
		}
	}
}

MooreMachine MooreMachine::FromDotFile(const std::string& name)
{
	std::ifstream file(name);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + name);
	}

	MooreMachine machine;
	ParseDotMoore(machine, file);

	return machine;
}

std::string MooreMachine::ToDotString() const
{
	std::ostringstream oss;
	oss << "digraph MooreMachine {" << std::endl;

	for (const auto& state : m_states)
	{
		const std::string& output = m_outputs.at(state);
		oss << state << " [label = \"" << state << "/" << output << "\"]" << std::endl;
	}
	oss << std::endl;

	std::vector<std::tuple<State, State, std::string>> sortedTransitions;
	for (const auto& trans : m_transitions)
	{
		sortedTransitions.emplace_back(trans.first.first, trans.second, trans.first.second);
	}
	std::ranges::sort(sortedTransitions);

	for (const auto& transition : sortedTransitions)
	{
		oss << std::get<0>(transition) << " -> " << std::get<1>(transition) << " [label = \"" << std::get<2>(transition) << "\"]" << std::endl;
	}

	oss << "}" << std::endl;
	return oss.str();
}

std::string MooreMachine::Print() const
{
	if (m_states.empty())
	{
		return "Moore Machine is empty";
	}

	std::ostringstream oss;
	std::set<std::string> inputs;
	for (const auto& transition : m_transitions)
	{
		inputs.insert(transition.first.second);
	}

	std::vector<State> states(m_states.begin(), m_states.end());
	std::ranges::sort(states);

	oss << "Moore machine table" << std::endl;
	oss << "Start state: " << m_startState << std::endl << std::endl;

	oss << std::setw(STATE_WIDTH) << std::left << "Input/State";
	for (const auto& state : states)
	{
		oss << std::setw(CELL_WIDTH) << std::left << state;
	}
	oss << std::endl << std::setw(STATE_WIDTH) << std::left << "Output";
	for (const auto& state : states)
	{
		oss << std::setw(CELL_WIDTH) << std::left << m_outputs.at(state);
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
				oss << std::setw(CELL_WIDTH) << std::left << it->second;
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

std::set<State> MooreMachine::GetStates() const
{
	return m_states;
}
State MooreMachine::GetStartState() const
{
	return m_startState;
}
MooreMachine::MooreOutputs MooreMachine::GetOutputs() const
{
	return m_outputs;
}
MooreMachine::MooreTransitions MooreMachine::GetTransitions() const
{
	return m_transitions;
}

void MooreMachine::AddState(const State& state, const std::string& output)
{
	m_states.insert(state);
	m_outputs[state] = output;
}

void MooreMachine::SetStartState(const State& state)
{
	if (!m_states.contains(state))
	{
		throw std::invalid_argument("State " + state + " is not in the machine");
	}
	m_startState = state;
}

void MooreMachine::SetTransition(const State& fromState, const std::string& input, const State& toState)
{
	if (!m_states.contains(fromState) || !m_states.contains(toState))
	{
		throw std::invalid_argument("One of the states in transition is not in the machine");
	}
	m_transitions[{fromState, input}] = toState;
}

void MooreMachine::SetStateOutput(const State& state, const std::string& output)
{
	if (!m_states.contains(state))
	{
		throw std::invalid_argument("State " + state + " is not in the machine");
	}
	m_outputs[state] = output;
}
