#include "stdlib.h"

#include "milk.h"

struct data_size {
	char d[2] = "0";
	char* data() { return d; };
	std::size_t size() { return 2; };
};

int main(int argc, char* argv[]) {

	data_size ds;
	std::string str = "foo bar";

	milk::bite data;
	data.set(ds);
	//data.set(2048);
	//long i = data.get<int>();
	//float d = data.get<float>();
	//
	//std::vector<unsigned char> ch_vec = data.get<std::vector<unsigned char>>();
	return EXIT_SUCCESS;
}