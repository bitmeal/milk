#include "stdlib.h"

#include <vector>
#include <string>
#include <map>

#include <iostream>

#include <cassert>
#include <exception>

#include "milk.h"



int main(int argc, char* argv[]) {

	milk::bite data;


	//data["level02"]["level11"] = "bar";
	//data["level02"]["level12"] = std::vector<std::string>{ "foo", "bar", "buz" };
	//data["level02"]["level13"] = 9999;
	//data["level02"]["level14"]["$level21"] = "escape";
	//data["level02"]["level14"]["level22"] = 3.14;
	//auto dataFlat = data.flatten('.', '$');


	data["level01"] = "foo";
	data["level02"] = true;
	data["level03"]["level11"] = "foobar";
	data["level03"]["level12"] = 9999;

	*(data.begin()) = milk::bite(1337);
	
	return EXIT_SUCCESS;
}