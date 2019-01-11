#include "stdlib.h"

#include <vector>
#include <string>
#include <map>

#include <iostream>

#include <cassert>
#include <exception>

#include "milk.h"


//std::cout << " TEST" << std::endl;
//assert();
//std::cout << "TEST OK!" << std::endl << std::endl;


int main(int argc, char* argv[]) {


	std::cout << "BASIC TESTS" << std::endl << std::endl;

	std::cout << "CONSTRUCTABLE TEST" << std::endl;
	try {
		milk::bite data = milk::bite();
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;




	milk::bite data;

	std::cout << "EMPTY SIZE TEST" << std::endl;
	try {
		assert(data.empty());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "ASIGNMENT/COPY OF EMPTY BITE TEST" << std::endl;
	try {
		data = milk::bite();
		assert(data.empty());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "INT TEST" << std::endl;
	try {
		data = (int)1;
		assert(data.get<int>() == 1);
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "DOUBLE TEST" << std::endl;
	try {
		data = (double)1.;
		assert(data.get<double>() == (double)1.);
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "CHAR/BYTE TEST" << std::endl;
	try {
		data = (unsigned char)'b';
		assert(data.get<unsigned char>() == (unsigned char)'b');
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "STRING TEST" << std::endl;
	try {
		std::string str = "foo bar";
		data = str;
		assert(data.get<std::string>() == "foo bar");
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "STRING FROM char* TEST 1" << std::endl;
	try {
		char* str = "foo bar";
		data = str;
		assert(data.get<std::string>() == "foo bar");
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "STRING FROM char* TEST 2" << std::endl;
	try {
		data = "foo bar";
		assert(data.get<std::string>() == "foo bar");
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "BOOL TEST" << std::endl;
	try {
		data = true;
		assert(data.get<bool>());
		data = false;
		assert(!data.get<bool>());
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "BINARY TEST" << std::endl;
	try {
		std::string compare_str = "binary";
		std::vector<unsigned char> compare_char_vec(compare_str.begin(), compare_str.end());
		// char array gets \0 terminated, size = 7; binary data size = 6!
		struct generic_binary_t {
			char d[7] = "binary";
			const char* data() const { return d; };
			std::size_t size() const { return 6; };
		} generic_binary;
		data = generic_binary;
		assert(data.get<std::vector<unsigned char>>() == compare_char_vec);
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "SIZE TEST" << std::endl;
	try {
		std::cout << "STILL == 1 AFTER MULTIPLE REASSIGMENTS" << std::endl;
		std::cout << "SIZE IS SIZE OF THE STRUCTURE; ONLY MAP AND LIST CAN BE < 1" << std::endl;
		assert(data.size() == 1);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;


	std::cout << "DATA CONVERSION TESTS" << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;


	std::cout << "DATA-STRUCTURES TESTS" << std::endl;

	//std::cout << "VECTOR TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP TEST" << std::endl;
	try {
		std::map<std::string, int> int_map{
			{ "foo", 1111 },
			{ "bar", 9999 },
			{ "key", 1}
		};
		data = int_map;
		assert(data.size() == 3);
		assert(data["foo"].get<int>() == 1111);
		assert(data["bar"].get<int>() == 9999);
		assert(data["key"].get<int>() == 1);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "NESTED MAP TEST" << std::endl;
	try {
		data["foo"]["nested"] = "chararr";
		assert(data.size() == 3);
		assert(data["foo"]["nested"].get<std::string>() == "chararr");
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "HETEROGENEOUS MAP TEST" << std::endl;
	try {
		data["bar"] = "chararr";
		assert(data.size() == 3);
		assert(data["bar"].get<std::string>() == "chararr");
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP CLEAR TEST" << std::endl;
	try {
		data.clear();
		assert(data.size() == 0);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;


	std::cout << "ADVANCED INTERFACE FUNCTION TESTS" << std::endl;

	std::cout << "MAP ITERATOR TEST" << std::endl;
	std::cout << "ACCESS BY PAIR AND PROJECTED VALUE" << std::endl;
	try {
		data.clear();
		data["foo"] = 1;
		data["bar"] = 0.6;
		data["parent"]["child"] = 9999;
		int count = 0;
		for (auto& it : data) {
			++count;
			if (count < 3) {
				// projected value
				assert(it.is_scalar());
				// std::pair
				assert(it.second.is_scalar());
			}
			if (count == 3) {
				// projected value
				assert(it.is_map());
				// std::pair
				assert(it.second.is_map());
			}
		}
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;


	return EXIT_SUCCESS;
}