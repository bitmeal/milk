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

	std::map<std::string, int> mi{
		{ "foo", 1337 },
		{ "bar", 9999 }
	};

	data = mi;
	std::cout << data.size() << std::endl;
	for (auto& miit : data) {
		std::cout << miit.first << ":" << miit.second.get<std::string>() << std::endl;
	};

	for (auto& miit : data) {
		std::cout << miit.first << ":" << miit.second.get<std::string>() << std::endl;
	};

	data["oof"] = 666;
	std::cout << data.size() << std::endl;
	std::cout << data["buz"].size() << std::endl;

	data["buz"]["biz"]["baz"] = 1010;
	std::cout << data.size() << std::endl;
	std::cout << data["buz"].size() << std::endl;
	std::cout << data["buz"]["biz"].size() << std::endl;

	bool buz = data["buz"]["biz"]["bar"].get<bool>();
	std::cout << buz << std::endl;

	/*
	for (auto& miit : data) {
		std::cout << miit.first << ":" << miit.second.get<std::string>() << std::endl;
	};
	*/

	/*
	data_size ds;
	data.set(ds);

	std::string str = "foo bar";
	data = str;

	data = 3;
	for (auto& sit : data) {
		std::cout << data.get<double>() << std::endl;
	}

	std::vector<double> vd{ 1., 2., 9., 0. };
	data = vd;
	for (auto& vdit : data) {
		std::cout << vdit.get<std::string>() << std::endl;
	};

	std::vector<char> vc{ 'f', 'o', 'o', '7' };
	//data.set(vc);

	std::map<std::string, int> mi{
		{"foo", 1337},
		{"bar", 9999}
	};
	data = mi;
	for (auto& miit : data) {
		std::cout << miit.first << ":" << miit.second.get<std::string>() << std::endl;
	};

	std::cout << data["foo"].get<std::string>() << std::endl;
	milk::bite buz = data["buz"];
	data("bar") = mi;
	std::cout << data["bar"]["bar"].get<std::string>() << std::endl;

	milk::bite rec;
	rec("bar")("foo") = 3.;
	std::cout << rec["bar"]["foo"].get<double>() << std::endl;

	milk::bite cp_data(data);
	*/
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