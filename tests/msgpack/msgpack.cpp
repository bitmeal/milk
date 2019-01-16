#include "stdlib.h"

#include <iostream>

#include <string>

#include <cassert>
#include <exception>

#include "milk.h"
#include "milk/modules/msgpack.h"
#include <msgpack.hpp>


int main(int argc, char* argv[]) {

	// development not test!

	msgpack::sbuffer msgpack_buffer;
	msgpack::packer<msgpack::sbuffer> msgpack_packer(msgpack_buffer);


	msgpack_packer.pack_map(2);
	msgpack_packer.pack("key0");
	msgpack_packer.pack("bar");
	msgpack_packer.pack("key1");
	msgpack_packer.pack(9999);

	msgpack_packer.pack_array(4);
	msgpack_packer.pack("first");
	msgpack_packer.pack(2);
	msgpack_packer.pack(3);
	msgpack_packer.pack("last");

	msgpack_packer.pack(1337);
	msgpack_packer.pack("foo");

	std::vector<char> bin_data = {'f', 'o', 'o', ' ', 'b', 'a', 'r'};
	msgpack::type::ext ext_bin(42, bin_data.data(), bin_data.size());
	msgpack_packer.pack(ext_bin);

	
	milk::bite data = milk::msgpack_module::from(msgpack_buffer.data(), msgpack_buffer.size());

	msgpack::sbuffer to_msgback_buffer;
	
	milk::msgpack_module::to(data, to_msgback_buffer);

	return EXIT_SUCCESS;
}