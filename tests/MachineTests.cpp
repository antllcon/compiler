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
