#pragma once

namespace FMachine
{
class MealyMachine;
class MooreMachine;
} // namespace FMachine

namespace FMachine::Converter
{

MooreMachine ToMoore(const MealyMachine& mealy);
MealyMachine ToMealy(const MooreMachine& moore);

} // namespace fa::Converter