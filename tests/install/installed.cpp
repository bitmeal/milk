#include "stdlib.h"

#include <vector>
#include <iostream>

#include "milk.h"

int main(int argc, char* argv[]) {

	std::vector<std::string> milk_vec{ "milk", "is", "installed" };
	
	milk::bite data = milk_vec;
	
	for( auto& it : data)
	{
		std::cout << it.get<std::string>() << std::endl;
	}
	
	return EXIT_SUCCESS;
}