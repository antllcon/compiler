#include <MealyMachine.h>
#include <MooreMachine.h>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <ostream>

int main()
{
	try
	{
		// const std::string filename = "../../../res/moore.dot";
		// const MooreMachine moore = MooreMachine::FromDotFile(filename);
		// std::cout << moore.Print() << std::endl;

		const std::string filename = "C:/Users/antllcon/source/repos/compiler/res/mealy.dot";

		std::cout << "========== ORIGINAL MEALY MACHINE ==========" << std::endl;
		MealyMachine mealy = MealyMachine::FromDotFile(filename);
		std::cout << mealy.Print() << std::endl;

		std::cout << "========== CONVERTED TO MOORE MACHINE ======" << std::endl;
		MooreMachine moore(mealy);
		std::cout << moore.Print() << std::endl;

		std::cout << "========== CONVERTED BACK TO MEALY MACHINE =" << std::endl;
		MealyMachine convertedMealy(moore);
		std::cout << convertedMealy.Print() << std::endl;

		std::cout << "========== CONVERTED TO MOORE MACHINE ======" << std::endl;
		MooreMachine mooreConverted(convertedMealy);
		std::cout << mooreConverted.Print() << std::endl;


	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
