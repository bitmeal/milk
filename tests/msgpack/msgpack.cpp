#include "stdlib.h"

#include <iostream>

#include <string>

#include <cassert>
#include <exception>

#include "milk.h"
#include "milk/modules/msgpack.h"
#include <msgpack.hpp>


int main(int argc, char* argv[]) {

	msgpack::sbuffer msgpack_buffer;
	msgpack::packer<msgpack::sbuffer> msgpack_packer(msgpack_buffer);

	msgpack_packer.pack(1337);
	msgpack_packer.pack("foo");

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









	return EXIT_SUCCESS;
}