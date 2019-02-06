#include "stdlib.h"

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <functional>

#include <iostream>

#include <cassert>
#include <exception>

#include "milk.h"


//std::cout << " TEST" << std::endl;
//try {
//
//assert();
//}
//catch (...) { assert(false); }
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

	std::cout << "STRING FROM char* TEST #1" << std::endl;
	try {
		char* str = "foo bar";
		data = str;
		assert(data.get<std::string>() == "foo bar");
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "STRING FROM char* TEST #2" << std::endl;
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

	std::cout << "SIZE TEST #1" << std::endl;
	try {
		data = true;
		std::cout << "STILL == 1 AFTER MULTIPLE REASSIGMENTS" << std::endl;
		std::cout << "SIZE IS SIZE OF THE STRUCTURE OR BINARY/STRING DATA; SCALAR.size() == 1" << std::endl;
		assert(data.size() == 1);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "BINARY vector<unsigned char> TEST" << std::endl;
	try {
		std::string source_str = "binary";
		std::vector<unsigned char> binary_char_vec(source_str.begin(), source_str.end());
		// char array gets \0 terminated, size = 7; binary data size = 6!
		data = binary_char_vec;
		assert(data.is_scalar());
		assert(data.get<std::vector<unsigned char>>() == binary_char_vec);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "GENERIC BINARY CLASS TEST" << std::endl;
	try {
		std::string compare_str = "binary";
		std::vector<unsigned char> compare_char_vec(compare_str.begin(), compare_str.end());
		// char array gets \0 terminated, size = 7; binary data size = 6!
		struct generic_binary_t {
			char d[7] = "binary";
			const unsigned char* data() const { return (unsigned char*)d; };
			std::size_t size() const { return 6; };
		} generic_binary;
		data = generic_binary;
		assert(data.get<std::vector<unsigned char>>() == compare_char_vec);
		assert(data.is_scalar());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "SIZE TEST #2" << std::endl;
	try {
		std::cout << "SIZE IS SIZE OF BINARY DATA" << std::endl;

		struct generic_binary_t {
			char d[7] = "binary";
			const unsigned char* data() const { return (unsigned char*)d; };
			std::size_t size() const { return 6; };
		} generic_binary;
		data = generic_binary;

		assert(data.size() == 6);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;


	std::cout << "DATA CONVERSION TESTS" << std::endl;

	//std::cout << " TEST" << std::endl;
	//assert();
	//std::cout << "TEST OK!" << std::endl << std::endl;


	std::cout << "DATA-STRUCTURES TESTS" << std::endl;

	std::vector<int> int_vec{ 1, 2, 3, 4, 5 };

	std::cout << "VECTOR ASSIGN TEST" << std::endl;
	try {
		data = int_vec;
		assert(data.size() == 5);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "VECTOR ELEMENT ACCESS TEST" << std::endl;
	try {
		data = int_vec;
		assert(data[0].get<int>() == 1);
		assert(data[1].get<int>() == 2);
		assert(data[2].get<int>() == 3);
		assert(data[3].get<int>() == 4);
		assert(data[4].get<int>() == 5);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "VECTOR HETEROGENEOUS TEST" << std::endl;
	try {
		data = int_vec;
		data[2] = "foobar";
		assert(data[0].get<int>() == 1);
		assert(data[1].get<int>() == 2);
		assert(data[2].get<std::string>() == "foobar");
		assert(data[3].get<int>() == 4);
		assert(data[4].get<int>() == 5);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "VECTOR POP_BACK TEST" << std::endl;
	try {
		data = int_vec;
		data.pop_back();
		assert(data.size() == 4);
		assert(data[0].get<int>() == 1);
		assert(data[1].get<int>() == 2);
		assert(data[2].get<int>() == 3);
		assert(data[3].get<int>() == 4);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "VECTOR PUSH_BACK TEST (INT)" << std::endl;
	try {
		data = int_vec;
		data.push_back(6);
		assert(data.size() == 6);
		assert(data[4].get<int>() == 5);
		assert(data[5].get<int>() == 6);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	//std::cout << " TEST" << std::endl;
	//try {
	//
	//assert();
	//}
	//catch (...) { assert(false); }
	//std::cout << "TEST OK!" << std::endl << std::endl;


	std::map<std::string, int> int_map{
		{ "foo", 1111 },
		{ "bar", 9999 },
		{ "key", 1}
	};

	std::cout << "MAP ASSIGN TEST" << std::endl;
	try {
		data = int_map;
		assert(data.size() == 3);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ELEMENT ACCESS TEST" << std::endl;
	try {
		data = int_map;
		assert(data["foo"].get<int>() == 1111);
		assert(data["bar"].get<int>() == 9999);
		assert(data["key"].get<int>() == 1);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP PROBE DOES NOT CREATE TEST" << std::endl;
	try {
		data = int_map;
		assert(data["non_ext"].size() == 0);
		assert(data.size() == 3);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP CLEAR TEST" << std::endl;
	try {
		data = int_map;
		data.clear();
		assert(data.size() == 0);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "NESTED MAP TEST" << std::endl;
	try {
		data = int_map;
		data["foo"]["nested"] = "chararr";
		assert(data.size() == 3);
		assert(data["foo"]["nested"].get<std::string>() == "chararr");
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "HETEROGENEOUS MAP TEST" << std::endl;
	try {
		data = int_map;
		data["bar"] = "chararr";
		assert(data.size() == 3);
		assert(data["bar"].get<std::string>() == "chararr");
		assert(data["foo"].get<int>() == 1111);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP FIND TEST" << std::endl;
	try {
		data = int_map;
		auto found_it = data.find("foo");
		assert(found_it->get<int>() == 1111);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ERASE BY ITERATOR TEST" << std::endl;
	try {
		data = int_map;
		auto found_it = data.find("foo");
		auto next_it = found_it; ++next_it;
		data.erase(found_it, next_it);
		assert(data.size() == 2);
		assert(data["foo"].empty());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ERASE BY ITERATOR TIL END TEST" << std::endl;
	try {
		data = int_map;
		auto found_it = data.find("foo");
		data.erase(found_it, data.end());
		assert(data.size() == 1);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ERASE BY ITERATOR BEGIN END TEST" << std::endl;
	try {
		data = int_map;
		data.erase(data.begin(), data.end());
		assert(data.empty());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ERASE BY KEY TEST" << std::endl;
	try {
		data = int_map;
		data.erase("foo");
		assert(data.size() == 2);
		assert(data["foo"].empty());
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;


	//std::cout << " TEST" << std::endl;
	//try {
	//
	//assert();
	//}
	//catch (...) { assert(false); }
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
		for (auto&& it : data) {
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

	std::cout << "MAP ITERATOR TEST" << std::endl;
	std::cout << "MODIFY ITERATED STRUCTURE" << std::endl;
	try {
		data.clear();
		data["foo"] = 1;
		data["bar"] = 0.6;
		data["parent"]["child"] = 9999;
		data["parent"]["sibling"] = "foo";

		for (auto&& it : data) {
			if(it.is_scalar())
				it = "bar";
		}

		for (auto&& it : data["parent"]) {
			if(it.is_scalar())
				it = "bar";
		}

		std::string str_comp = "bar";
		assert(data["foo"].get<std::string>() == str_comp);
		assert(data["bar"].get<std::string>() == str_comp);
		assert(data["parent"]["child"].get<std::string>() == str_comp);
		assert(data["parent"]["sibling"].get<std::string>() == str_comp);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP ITERATOR TEST: for_each" << std::endl;
	std::cout << "USING ITERATOR BASED STD::UTILITY" << std::endl;
	try {
		data.clear();
		data["foo"] = 1;
		data["bar"] = 0.6;
		data["parent"]["child"] = 9999;
		data["parent"]["sibling"]["grandkid"] = "foobar";
		data["parent"]["sibling"]["sibling"] = 1234;

		std::function<void(milk::bite&)> recfun = [&recfun](milk::bite& data) {
			if (data.is_map() || data.is_list())
			{
				std::for_each(data.begin(), data.end(), recfun);
				return;
			}

			data = "bar";
		};
		std::for_each(data.begin(), data.end(), recfun);

		std::string str_comp = "bar";
		assert(data["foo"].get<std::string>() == str_comp);
		assert(data["bar"].get<std::string>() == str_comp);
		assert(data["parent"]["child"].get<std::string>() == str_comp);
		assert(data["parent"]["sibling"]["grandkid"].get<std::string>() == str_comp);
		assert(data["parent"]["sibling"]["sibling"].get<std::string>() == str_comp);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP FLATTEN TEST" << std::endl;
	try {
		data.clear();
		//alphabetical order!
		data["a_foo"] = 1;
		data["b_bar"] = 0.6;
		data["c_parent"]["a_child"] = 9999;
		data["c_parent"]["a_sibling"] = "foo";

		milk::bite flat = data.flatten();

		//alphabetical order!
		std::vector<std::string> compare = {"a_foo", "b_bar", "c_parent.a_child", "c_parent.a_sibling"};

		auto flatIt = flat.begin();
		auto compareIt = compare.begin();

		assert(flat.is_map());
		assert(flat.size() == 4);
		for(; flatIt != flat.end() && compareIt != compare.end(); ++compareIt,++flatIt)
			assert(flatIt->first == *compareIt);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP FLATTEN TEST" << std::endl;
	std::cout << "USING SPECIFIED DELIMITER: '/'" << std::endl;
	try {
		data.clear();
		//alphabetical order!
		data["a_foo"] = 1;
		data["b_bar"] = 0.6;
		data["c_parent"]["a_child"] = 9999;
		data["c_parent"]["a_sibling"] = "foo";

		milk::bite flat = data.flatten('/');

		//alphabetical order!
		std::vector<std::string> compare = {"a_foo", "b_bar", "c_parent/a_child", "c_parent/a_sibling"};

		auto flatIt = flat.begin();
		auto compareIt = compare.begin();

		assert(flat.is_map());
		assert(flat.size() == 4);
		for(; flatIt != flat.end() && compareIt != compare.end(); ++compareIt,++flatIt)
			assert(flatIt->first == *compareIt);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;

	std::cout << "MAP FLATTEN TEST" << std::endl;
	std::cout << "PROTECT KEYS BY PREFIX" << std::endl;
	try {
		data.clear();
		//alphabetical order!
		data["a_foo"] = 1;
		data["b_bar"] = 0.6;
		data["c_parent"]["$a_child"] = 9999; // <-- protect by $ character
		data["c_parent"]["a_sibling"] = "foo";

		milk::bite flat = data.flatten('.', '$');

		//alphabetical order!
		std::vector<std::string> compare = {"a_foo", "b_bar", "c_parent", "c_parent.a_sibling"};

		auto flatIt = flat.begin();
		auto compareIt = compare.begin();

		assert(flat.is_map());
		assert(flat.size() == 4);
		assert(flat["c_parent"].is_map());
		assert(flat["c_parent"]["$a_child"].get<int>() == 9999);
		for(; flatIt != flat.end() && compareIt != compare.end(); ++compareIt,++flatIt)
			assert(flatIt->first == *compareIt);
	}
	catch (...) { assert(false); }
	std::cout << "TEST OK!" << std::endl << std::endl;


	//std::cout << " TEST" << std::endl;
	//try {
	//
	//assert();
	//}
	//catch (...) { assert(false); }
	//std::cout << "TEST OK!" << std::endl << std::endl;


	return EXIT_SUCCESS;
}
