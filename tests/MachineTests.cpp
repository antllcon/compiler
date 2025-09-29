#include "../libs/FiniteAutomation/src/MealyMachine.cpp"
#include "MealyMachine.h"
#include "MooreMachine.h"
#include "gtest/gtest.h"

TEST(MealyMachineTest, CanCreateEmptyMachine)
{
	MealyMachine machine;
	EXPECT_TRUE(machine.GetStates().empty());
	EXPECT_TRUE(machine.GetTransitions().empty());
	EXPECT_EQ(machine.GetStartState(), "");
}

TEST(MealyMachineTest, CanAddState)
{
	MealyMachine machine;
	machine.AddState("S1");
	EXPECT_EQ(machine.GetStates().size(), 1);
	EXPECT_TRUE(machine.GetStates().contains("S1"));
}

TEST(MealyMachineTest, CanSetAndGetStartState)
{
	MealyMachine machine;
	machine.AddState("S1");
	machine.SetStartState("S1");
	EXPECT_EQ(machine.GetStartState(), "S1");
}

TEST(MealyMachineTest, SetTransitionAddsStatesAutomatically)
{
	MealyMachine machine;
	machine.SetTransition("S1", "a", "S2", "b");
	EXPECT_EQ(machine.GetStates().size(), 2);
	EXPECT_TRUE(machine.GetStates().contains("S1"));
	EXPECT_TRUE(machine.GetStates().contains("S2"));
}

TEST(MooreMachineTest, CanCreateEmptyMachine)
{
	MooreMachine machine;
	EXPECT_TRUE(machine.GetStates().empty());
	EXPECT_TRUE(machine.GetTransitions().empty());
	EXPECT_TRUE(machine.GetOutputs().empty());
	EXPECT_EQ(machine.GetStartState(), "");
}

TEST(MooreMachineTest, CanAddStateWithOutput)
{
	MooreMachine machine;
	machine.AddState("S1", "o1");
	EXPECT_EQ(machine.GetStates().size(), 1);
	EXPECT_TRUE(machine.GetStates().contains("S1"));
	EXPECT_EQ(machine.GetOutputs().at("S1"), "o1");
}

TEST(MooreMachineTest, CanSetAndGetStartState)
{
	MooreMachine machine;
	machine.AddState("S1", "o1");
	machine.SetStartState("S1");
	EXPECT_EQ(machine.GetStartState(), "S1");
}

TEST(ConversionTest, MealyToMooreConversionIsCorrect)
{
	MealyMachine mealy;
	mealy.AddState("S0");
	mealy.AddState("S1");
	mealy.AddState("S2");

	mealy.SetStartState("S0");
	mealy.SetTransition("S0", "0", "S1", "1");
	mealy.SetTransition("S0", "1", "S2", "0");
	mealy.SetTransition("S1", "0", "S2", "0");
	mealy.SetTransition("S1", "1", "S0", "1");
	mealy.SetTransition("S2", "0", "S0", "1");
	mealy.SetTransition("S2", "1", "S1", "0");

	MooreMachine moore(mealy);

	EXPECT_EQ(moore.GetStates().size(), 5);
	EXPECT_TRUE(moore.GetStates().contains("S0"));
	EXPECT_TRUE(moore.GetStates().contains("S1_1"));
	EXPECT_TRUE(moore.GetStates().contains("S2_0"));

	auto transitions = moore.GetTransitions();
	EXPECT_EQ(transitions.size(), 10);
	EXPECT_EQ(transitions.at({"S0", "0"}), "S1_1");
	EXPECT_EQ(transitions.at({"S0", "1"}), "S2_0");
	EXPECT_EQ(transitions.at({"S1_1", "0"}), "S2_0");
	EXPECT_EQ(transitions.at({"S1_1", "1"}), "S0_1");
	EXPECT_EQ(transitions.at({"S2_0", "0"}), "S0_1");
	EXPECT_EQ(transitions.at({"S2_0", "1"}), "S1_0");
}

TEST(ConversionTest, MooreToMealyConversionIsCorrect)
{
	MooreMachine moore;
	moore.AddState("S0", "0");
	moore.AddState("S1", "1");
	moore.AddState("S2", "1");

	moore.SetStartState("S0");
	moore.SetTransition("S0", "a", "S1");
	moore.SetTransition("S0", "b", "S2");
	moore.SetTransition("S1", "a", "S0");
	moore.SetTransition("S1", "b", "S2");
	moore.SetTransition("S2", "a", "S1");
	moore.SetTransition("S2", "b", "S0");

	MealyMachine mealy(moore);

	EXPECT_EQ(mealy.GetStates().size(), 3);
	EXPECT_TRUE(mealy.GetStates().contains("S0"));
	EXPECT_TRUE(mealy.GetStates().contains("S1"));
	EXPECT_TRUE(mealy.GetStates().contains("S2"));

	auto transitions = mealy.GetTransitions();
	EXPECT_EQ(transitions.size(), 6);
	EXPECT_EQ(transitions.at({"S0", "a"}), std::make_pair("S1", "1"));
	EXPECT_EQ(transitions.at({"S0", "b"}), std::make_pair("S2", "1"));
	EXPECT_EQ(transitions.at({"S1", "a"}), std::make_pair("S0", "0"));
	EXPECT_EQ(transitions.at({"S1", "b"}), std::make_pair("S2", "1"));
	EXPECT_EQ(transitions.at({"S2", "a"}), std::make_pair("S1", "1"));
	EXPECT_EQ(transitions.at({"S2", "b"}), std::make_pair("S0", "0"));
}

// Минимизация Милли
TEST(MealyMachineMinimizationTest, EmptyMachineMinimization)
{
	MealyMachine machine;
	MealyMachine minimized = machine.Minimize();

	EXPECT_TRUE(minimized.GetStates().empty());
	EXPECT_TRUE(minimized.GetTransitions().empty());
	EXPECT_EQ(minimized.GetStartState(), "");
}

TEST(MealyMachineMinimizationTest, SingleStateMachineMinimization)
{
	MealyMachine machine;
	machine.AddState("S0");
	machine.SetStartState("S0");

	MealyMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 1);
	EXPECT_TRUE(minimized.GetStates().contains("S0"));
	EXPECT_EQ(minimized.GetStartState(), "S0");
	EXPECT_TRUE(minimized.GetTransitions().empty());
}

TEST(MealyMachineMinimizationTest, TwoEquivalentStatesMinimization)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S0", "a");
	machine.SetTransition("S0", "1", "S0", "b");
	machine.SetTransition("S1", "0", "S1", "a");
	machine.SetTransition("S1", "1", "S1", "b");

	MealyMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 1);
	EXPECT_EQ(minimized.GetTransitions().size(), 2);

	auto transitions = minimized.GetTransitions();
	State remainingState = *minimized.GetStates().begin();

	EXPECT_TRUE(transitions.contains({remainingState, "0"}));
	EXPECT_TRUE(transitions.contains({remainingState, "1"}));
	EXPECT_EQ(transitions.at({remainingState, "0"}).second, "a");
	EXPECT_EQ(transitions.at({remainingState, "1"}).second, "b");
}

TEST(MealyMachineMinimizationTest, DistinctStatesNotMerged)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S0", "a");
	machine.SetTransition("S0", "1", "S0", "b");
	machine.SetTransition("S1", "0", "S1", "x");
	machine.SetTransition("S1", "1", "S1", "b");

	MealyMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 2);
	EXPECT_EQ(minimized.GetTransitions().size(), 4);
}

TEST(MealyMachineMinimizationTest, ComplexMinimizationExample)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.AddState("S2");
	machine.AddState("S3");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S1", "a");
	machine.SetTransition("S0", "1", "S2", "b");
	machine.SetTransition("S1", "0", "S3", "c");
	machine.SetTransition("S1", "1", "S3", "d");
	machine.SetTransition("S2", "0", "S3", "c");
	machine.SetTransition("S2", "1", "S3", "d");
	machine.SetTransition("S3", "0", "S1", "e");
	machine.SetTransition("S3", "1", "S2", "f");

	MealyMachine minimized = machine.Minimize();

	EXPECT_LE(minimized.GetStates().size(), 3);
	EXPECT_LE(minimized.GetTransitions().size(), 6);

	EXPECT_TRUE(minimized.GetStates().contains("S0"));
}

TEST(MealyMachineMinimizationTest, StartStatePreservedAfterMinimization)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.AddState("S2");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S1", "x");
	machine.SetTransition("S0", "b", "S2", "y");
	machine.SetTransition("S1", "a", "S0", "z");
	machine.SetTransition("S2", "a", "S0", "z");

	MealyMachine minimized = machine.Minimize();

	EXPECT_FALSE(minimized.GetStartState().empty());
	EXPECT_TRUE(minimized.GetStates().contains(minimized.GetStartState()));
}

TEST(MealyMachineMinimizationTest, AllTransitionsPreservedAfterMinimization)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S1", "a");
	machine.SetTransition("S0", "1", "S0", "b");
	machine.SetTransition("S1", "0", "S0", "c");
	machine.SetTransition("S1", "1", "S1", "d");

	MealyMachine minimized = machine.Minimize();

	auto transitions = minimized.GetTransitions();
	auto states = minimized.GetStates();

	for (const auto& state : states)
	{
		EXPECT_TRUE(transitions.contains({state, "0"}));
		EXPECT_TRUE(transitions.contains({state, "1"}));
	}
}

TEST(MealyMachineMinimizationTest, IdempotentMinimization)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S1", "a");
	machine.SetTransition("S0", "1", "S0", "b");
	machine.SetTransition("S1", "0", "S0", "c");
	machine.SetTransition("S1", "1", "S1", "d");

	MealyMachine minimizedOnce = machine.Minimize();
	MealyMachine minimizedTwice = minimizedOnce.Minimize();

	EXPECT_EQ(minimizedOnce.GetStates().size(), minimizedTwice.GetStates().size());
	EXPECT_EQ(minimizedOnce.GetTransitions().size(), minimizedTwice.GetTransitions().size());
	EXPECT_EQ(minimizedOnce.GetStartState(), minimizedTwice.GetStartState());
}

TEST(MealyMachineMinimizationTest, MinimizationWithNoTransitions)
{
	MealyMachine machine;
	machine.AddState("S0");
	machine.AddState("S1");
	machine.AddState("S2");
	machine.SetStartState("S0");

	MealyMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 1);
	EXPECT_EQ(minimized.GetStartState(), "S0");
	EXPECT_TRUE(minimized.GetTransitions().empty());
}

TEST(MealyMachineMinimizationTest, VerifyMinimizedMachineFunctionality)
{
	MealyMachine machine;

	machine.AddState("S0");
	machine.AddState("S1");
	machine.AddState("S2");
	machine.AddState("S3");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S1", "out1");
	machine.SetTransition("S0", "b", "S2", "out2");
	machine.SetTransition("S1", "a", "S3", "out3");
	machine.SetTransition("S1", "b", "S3", "out4");
	machine.SetTransition("S2", "a", "S3", "out3");
	machine.SetTransition("S2", "b", "S3", "out4");
	machine.SetTransition("S3", "a", "S1", "out5");
	machine.SetTransition("S3", "b", "S2", "out6");

	MealyMachine minimized = machine.Minimize();

	auto transitions = minimized.GetTransitions();

	for (const auto& state : minimized.GetStates())
	{
		for (const auto& input : {"a", "b"})
		{
			EXPECT_TRUE(transitions.contains({state, input}));
			auto transition = transitions.at({state, input});
			EXPECT_TRUE(minimized.GetStates().contains(transition.first));
			EXPECT_FALSE(transition.second.empty());
		}
	}
}

TEST(MealyMachineMinimizationTest, SetTransitionAutomaticallyAddsStates)
{
	MealyMachine machine;

	machine.SetTransition("S0", "a", "S1", "x");
	machine.SetStartState("S0");

	MealyMachine minimized = machine.Minimize();

	EXPECT_GE(minimized.GetStates().size(), 1);
	EXPECT_EQ(minimized.GetStartState(), "S0");
}

// Минимизация Мура
TEST(MooreMachineMinimizationTest, EmptyMachineMinimization)
{
	MooreMachine machine;
	MooreMachine minimized = machine.Minimize();

	EXPECT_TRUE(minimized.GetStates().empty());
	EXPECT_TRUE(minimized.GetTransitions().empty());
	EXPECT_TRUE(minimized.GetOutputs().empty());
	EXPECT_EQ(minimized.GetStartState(), "");
}

TEST(MooreMachineMinimizationTest, SingleStateMachineMinimization)
{
	MooreMachine machine;
	machine.AddState("S0", "output0");
	machine.SetStartState("S0");

	MooreMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 1);
	EXPECT_TRUE(minimized.GetStates().contains("S0"));
	EXPECT_EQ(minimized.GetStartState(), "S0");
	EXPECT_EQ(minimized.GetOutputs().at("S0"), "output0");
	EXPECT_TRUE(minimized.GetTransitions().empty());
}

TEST(MooreMachineMinimizationTest, TwoEquivalentStatesMinimization)
{
	MooreMachine machine;

	machine.AddState("S0", "output");
	machine.AddState("S1", "output");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S0");
	machine.SetTransition("S0", "1", "S0");
	machine.SetTransition("S1", "0", "S1");
	machine.SetTransition("S1", "1", "S1");

	MooreMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 1);
	EXPECT_EQ(minimized.GetTransitions().size(), 2);
	EXPECT_EQ(minimized.GetOutputs().size(), 1);

	auto transitions = minimized.GetTransitions();
	State remainingState = *minimized.GetStates().begin();

	EXPECT_TRUE(transitions.contains({remainingState, "0"}));
	EXPECT_TRUE(transitions.contains({remainingState, "1"}));
	EXPECT_EQ(minimized.GetOutputs().at(remainingState), "output");
}

TEST(MooreMachineMinimizationTest, DistinctOutputsNotMerged)
{
	MooreMachine machine;

	machine.AddState("S0", "outputA");
	machine.AddState("S1", "outputB");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S0");
	machine.SetTransition("S0", "1", "S0");
	machine.SetTransition("S1", "0", "S1");
	machine.SetTransition("S1", "1", "S1");

	MooreMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 2);
	EXPECT_EQ(minimized.GetTransitions().size(), 4);
	EXPECT_EQ(minimized.GetOutputs().size(), 2);
}

TEST(MooreMachineMinimizationTest, ComplexMinimizationExample)
{
	MooreMachine machine;

	machine.AddState("S0", "o1");
	machine.AddState("S1", "o2");
	machine.AddState("S2", "o2");
	machine.AddState("S3", "o3");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S1");
	machine.SetTransition("S0", "b", "S2");
	machine.SetTransition("S1", "a", "S3");
	machine.SetTransition("S1", "b", "S3");
	machine.SetTransition("S2", "a", "S3");
	machine.SetTransition("S2", "b", "S3");
	machine.SetTransition("S3", "a", "S1");
	machine.SetTransition("S3", "b", "S2");

	MooreMachine minimized = machine.Minimize();

	EXPECT_LE(minimized.GetStates().size(), 3);
	EXPECT_LE(minimized.GetTransitions().size(), 6);

	EXPECT_TRUE(minimized.GetStates().contains("S0"));
	EXPECT_TRUE(minimized.GetOutputs().contains("S0"));
}

TEST(MooreMachineMinimizationTest, StartStatePreservedAfterMinimization)
{
	MooreMachine machine;

	machine.AddState("S0", "startOutput");
	machine.AddState("S1", "otherOutput");
	machine.AddState("S2", "otherOutput");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "x", "S1");
	machine.SetTransition("S0", "y", "S2");
	machine.SetTransition("S1", "x", "S0");
	machine.SetTransition("S2", "x", "S0");

	MooreMachine minimized = machine.Minimize();

	EXPECT_FALSE(minimized.GetStartState().empty());
	EXPECT_TRUE(minimized.GetStates().contains(minimized.GetStartState()));
	EXPECT_EQ(minimized.GetOutputs().at(minimized.GetStartState()), "startOutput");
}

TEST(MooreMachineMinimizationTest, AllTransitionsPreservedAfterMinimization)
{
	MooreMachine machine;

	machine.AddState("S0", "o1");
	machine.AddState("S1", "o2");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "0", "S1");
	machine.SetTransition("S0", "1", "S0");
	machine.SetTransition("S1", "0", "S0");
	machine.SetTransition("S1", "1", "S1");

	MooreMachine minimized = machine.Minimize();

	auto transitions = minimized.GetTransitions();
	auto states = minimized.GetStates();

	for (const auto& state : states)
	{
		EXPECT_TRUE(transitions.contains({state, "0"}));
		EXPECT_TRUE(transitions.contains({state, "1"}));
	}
}

TEST(MooreMachineMinimizationTest, IdempotentMinimization)
{
	MooreMachine machine;

	machine.AddState("S0", "output0");
	machine.AddState("S1", "output1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S1");
	machine.SetTransition("S0", "b", "S0");
	machine.SetTransition("S1", "a", "S0");
	machine.SetTransition("S1", "b", "S1");

	MooreMachine minimizedOnce = machine.Minimize();
	MooreMachine minimizedTwice = minimizedOnce.Minimize();

	EXPECT_EQ(minimizedOnce.GetStates().size(), minimizedTwice.GetStates().size());
	EXPECT_EQ(minimizedOnce.GetTransitions().size(), minimizedTwice.GetTransitions().size());
	EXPECT_EQ(minimizedOnce.GetStartState(), minimizedTwice.GetStartState());
	EXPECT_EQ(minimizedOnce.GetOutputs().size(), minimizedTwice.GetOutputs().size());
}

TEST(MooreMachineMinimizationTest, MinimizationWithNoTransitions)
{
	MooreMachine machine;
	machine.AddState("S0", "o1");
	machine.AddState("S1", "o1");
	machine.AddState("S2", "o2");
	machine.SetStartState("S0");

	MooreMachine minimized = machine.Minimize();

	EXPECT_LE(minimized.GetStates().size(), 2);
	EXPECT_EQ(minimized.GetStartState(), "S0");
	EXPECT_TRUE(minimized.GetTransitions().empty());
}

TEST(MooreMachineMinimizationTest, VerifyMinimizedMachineFunctionality)
{
	MooreMachine machine;

	machine.AddState("S0", "init");
	machine.AddState("S1", "work");
	machine.AddState("S2", "work");
	machine.AddState("S3", "final");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "start", "S1");
	machine.SetTransition("S0", "skip", "S2");
	machine.SetTransition("S1", "process", "S3");
	machine.SetTransition("S1", "reset", "S0");
	machine.SetTransition("S2", "process", "S3");
	machine.SetTransition("S2", "reset", "S0");
	machine.SetTransition("S3", "done", "S1");
	machine.SetTransition("S3", "back", "S2");

	MooreMachine minimized = machine.Minimize();

	auto transitions = minimized.GetTransitions();
	auto outputs = minimized.GetOutputs();
	auto states = minimized.GetStates();

	for (const auto& state : states)
	{
		EXPECT_TRUE(outputs.contains(state));
		EXPECT_FALSE(outputs.at(state).empty());
	}

	EXPECT_FALSE(transitions.empty());
}

TEST(MooreMachineMinimizationTest, DifferentOutputsPreventMerging)
{
	MooreMachine machine;

	machine.AddState("S0", "red");
	machine.AddState("S1", "blue");
	machine.AddState("S2", "red");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S1");
	machine.SetTransition("S0", "b", "S2");
	machine.SetTransition("S1", "a", "S0");
	machine.SetTransition("S1", "b", "S2");
	machine.SetTransition("S2", "a", "S1");
	machine.SetTransition("S2", "b", "S0");

	MooreMachine minimized = machine.Minimize();

	EXPECT_EQ(minimized.GetStates().size(), 2);
	EXPECT_EQ(minimized.GetOutputs().size(), 2);

	bool foundRed = false, foundBlue = false;
	for (const auto& output : minimized.GetOutputs())
	{
		if (output.second == "red") foundRed = true;
		if (output.second == "blue") foundBlue = true;
	}
	EXPECT_TRUE(foundRed);
	EXPECT_TRUE(foundBlue);
}

TEST(MooreMachineMinimizationTest, SetTransitionAutomaticallyAddsStates)
{
	MooreMachine machine;

	machine.AddState("S0", "output0");
	machine.AddState("S1", "output1");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "input", "S1");

	MooreMachine minimized = machine.Minimize();

	EXPECT_GE(minimized.GetStates().size(), 1);
	EXPECT_EQ(minimized.GetStartState(), "S0");
	EXPECT_TRUE(minimized.GetOutputs().contains("S0"));
}

TEST(MooreMachineMinimizationTest, MultipleEquivalentStatesMerged)
{
	MooreMachine machine;

	machine.AddState("S0", "group1");
	machine.AddState("S1", "group1");
	machine.AddState("S2", "group1");
	machine.AddState("S3", "group2");
	machine.SetStartState("S0");

	machine.SetTransition("S0", "a", "S3");
	machine.SetTransition("S0", "b", "S0");
	machine.SetTransition("S1", "a", "S3");
	machine.SetTransition("S1", "b", "S1");
	machine.SetTransition("S2", "a", "S3");
	machine.SetTransition("S2", "b", "S2");
	machine.SetTransition("S3", "a", "S0");
	machine.SetTransition("S3", "b", "S3");

	MooreMachine minimized = machine.Minimize();

	EXPECT_LE(minimized.GetStates().size(), 2);
	EXPECT_LE(minimized.GetTransitions().size(), 4);
}
