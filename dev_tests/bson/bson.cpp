#include "stdlib.h"

#include <iostream>

#include <cassert>
#include <exception>

#include "milk.h"
#include "milk/modules/bson.h"

//#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>

int main(int, char*[]) {
	
	//using bsoncxx::builder::basic::kvp;
	//using bsoncxx::builder::basic::document;
	//using bsoncxx::builder::basic::make_document;
	//
	//document document_builder{};
	//document_builder.append(kvp("key0", 1337));
	//document_builder.append(kvp("key1", 9999));
	//
	//document sub_doc_builder{};
	//sub_doc_builder.append(kvp("foo", "bar"));
	//sub_doc_builder.append(kvp("bar", 1111));
	//
	//document_builder.append(kvp("foo bar", sub_doc_builder.view()));
	//
	//std::cout << bsoncxx::to_json(document_builder.view()) << std::endl;
	
	auto doc = bsoncxx::builder::stream::document{};

	doc
		<< "key0" << 1337
		<< "key1" << 9999
		<< "foo bar"
			<< bsoncxx::builder::stream::open_document
			<< "foo" << "bar"
			<< "bar" << 1111
			<< bsoncxx::builder::stream::close_document
		<< "arrkey"
			<< bsoncxx::builder::stream::open_array
			<< 1 << "buz" << true << 3.14
			<< bsoncxx::builder::stream::close_array
		;

	std::cout << bsoncxx::to_json(doc.view()) << std::endl;

	milk::bite data = milk::bson_module::from(doc);
	std::string str = "foo bar as bin";
	milk::binary_proxy bin = { (unsigned char*)str.data(), str.size() };
	data["bin_key"] = bin;
	data["bin_key"].bin_extension(127);

	auto milk_doc = bsoncxx::builder::basic::document{};
	milk::bson_module::to(data, milk_doc);

	std::cout << bsoncxx::to_json(milk_doc.view()) << std::endl;

	return EXIT_SUCCESS;

}