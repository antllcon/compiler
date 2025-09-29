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
using Partition = std::vector<std::set<State>>;

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

int FindGroupIndex(const Partition& partition, const State& state)
{
	for (size_t i = 0; i < partition.size(); ++i)
	{
		if (partition[i].contains(state))
		{
			return static_cast<int>(i);
		}
	}

	return -1;
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

MealyMachine MealyMachine::Minimize() const
{
	if (m_states.empty())
	{
		return {};
	}

	std::set<std::string> inputs;
	for (const auto& trans : m_transitions)
	{
		inputs.insert(trans.first.second);
	}
	std::vector<std::string> sortedInputs(inputs.begin(), inputs.end());

	Partition partition;
	{
		std::map<std::map<std::string, std::string>, std::set<State>> groupsByOutput;
		for (const auto& state : m_states)
		{
			std::map<std::string, std::string> outputSignature;
			for (const auto& input : sortedInputs)
			{
				if (const auto it = m_transitions.find({state, input}); it != m_transitions.end())
				{
					outputSignature[input] = it->second.second;
				}
			}
			groupsByOutput[outputSignature].insert(state);
		}

		for (const auto& pair : groupsByOutput)
		{
			if (!pair.second.empty())
			{
				partition.push_back(pair.second);
			}
		}
	}

	Partition prevPartition;
	do
	{
		prevPartition = partition;
		partition.clear();

		for (const auto& group : prevPartition)
		{
			if (group.size() <= 1)
			{
				partition.push_back(group);
				continue;
			}

			std::map<std::vector<int>, std::set<State>> newGroups;
			for (const auto& state : group)
			{
				std::vector<int> transitionSignature;
				for (const auto& input : sortedInputs)
				{
					if (const auto it = m_transitions.find({state, input}); it != m_transitions.end())
					{
						transitionSignature.push_back(FindGroupIndex(prevPartition, it->second.first));
					}
					else
					{
						transitionSignature.push_back(-1);
					}
				}
				newGroups[transitionSignature].insert(state);
			}

			for (const auto& pair : newGroups)
			{
				partition.push_back(pair.second);
			}
		}

		std::ranges::sort(partition, [](const auto& a, const auto& b) { return *a.begin() < *b.begin(); });

	} while (partition.size() != prevPartition.size());

	MealyMachine minimizedMachine;
	std::map<State, State> oldStateToNewState;

	for (const auto& group : partition)
	{
		State representative = *group.begin();
		minimizedMachine.AddState(representative);

		for (const auto& oldState : group)
		{
			oldStateToNewState[oldState] = representative;
		}

		if (group.contains(m_startState))
		{
			minimizedMachine.SetStartState(representative);
		}
	}

	for (const auto& trans : m_transitions)
	{
		const State& fromOld = trans.first.first;
		const std::string& input = trans.first.second;
		const State& toOld = trans.second.first;
		const std::string& output = trans.second.second;

		const State& fromNew = oldStateToNewState.at(fromOld);
		const State& toNew = oldStateToNewState.at(toOld);

		if (!minimizedMachine.GetTransitions().contains({fromNew, input}))
		{
			minimizedMachine.SetTransition(fromNew, input, toNew, output);
		}
	}

	return minimizedMachine;
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
