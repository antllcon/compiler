#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>

int main()
{
	try
	{
	}
	catch (std::exception& e)
	{
		std::cerr << "Error:" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}