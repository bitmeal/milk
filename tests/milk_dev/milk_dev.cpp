#include "stdlib.h"

#include <vector>
#include <string>
#include <map>

#include <iostream>

#include "milk.h"

struct data_size {
	char d[2] = "0";
	const char* data() const { return d; };
	std::size_t size() const { return 2; };
};

int main(int argc, char* argv[]) {

	milk::bite data;

	data_size ds;
	data.set(ds);

	std::string str = "foo bar";
	data.set(str);

	std::vector<double> vd{ 1., 2., 9., 0. };
	data.set(vd);
	/*
	for (auto& vdit : data) {
		std::cout << vdit.get<std::string>() << std::endl;
	};
	*/
	std::vector<char> vc{ 'f', 'o', 'o', '7' };
	//data.set(vc);

	std::map<std::string, int> mi{
		{"foo", 1337},
		{"bar", 9999}
	};

	data.set(mi);
	milk::bite cp_data(data);
	/*
	for (auto& mit : data) {
		std::cout << mit.first << std::endl;
	};
	*/
	//data.set(10);
	//data.set(0.1);

	//data.set("bar fuu");
	//data.set(2048);
	//
	//std::vector<unsigned char> ch_vec = data.get<std::vector<unsigned char>>();
	return EXIT_SUCCESS;
}