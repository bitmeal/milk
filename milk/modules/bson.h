#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>


namespace milk
{
	struct bson_module {

	private:
		static inline milk::bite as_scalar(const bsoncxx::types::value& data)
		{
			switch (data.type())
			{
			case bsoncxx::type::k_null:
				return milk::bite();
				break;
			case bsoncxx::type::k_int32:
				return milk::bite(data.get_int32().value);
				break;
			case bsoncxx::type::k_int64:
				return milk::bite(data.get_int64().value);
				break;
			case bsoncxx::type::k_bool:
				return milk::bite(data.get_bool().value);
				break;
			case bsoncxx::type::k_double:
				return milk::bite(data.get_double().value);
				break;
			case bsoncxx::type::k_utf8: // string (using raw ascii and no interpretation of UTF-8 here)
				return milk::bite(data.get_utf8().value.to_string());
				break;
			case bsoncxx::type::k_binary:
			{
				const bsoncxx::types::b_binary& bin = data.get_binary();
				milk::bite bite = milk::binary_proxy(bin.bytes, bin.size);
				bite.bin_extension((uint8_t)(bin.sub_type));
				return bite;
				break;
			}
			case bsoncxx::type::k_date: // milliseconds unix time, int64
			{
				milk::bite bite = data.get_date().to_int64();
				bite.bin_extension(0x09); //http://bsonspec.org/spec.html
				return bite;
			}
				break;
			case bsoncxx::type::k_oid: // object-id - binary
			{
				milk::bite bite = milk::binary_proxy(reinterpret_cast<const unsigned char*>(data.get_oid().value.bytes()), (std::size_t) 12); // objectID is 12 bytes
				bite.bin_extension(0x07); //http://bsonspec.org/spec.html
				return bite;
				break;
			}
			case bsoncxx::type::k_regex: // string
			{
				milk::bite bite(data.get_regex().regex.to_string());
				bite.bin_extension(0x0b); //http://bsonspec.org/spec.html
				return bite;
				break;
			}
			case bsoncxx::type::k_code: // JS code - string
			{
				milk::bite bite(data.get_code().code.to_string());
				bite.bin_extension(0x0d); //http://bsonspec.org/spec.html
				return bite;
				break;
			}
			case bsoncxx::type::k_timestamp: // non std, internal, (defined as uint64): first 32 bit = unix timestamp, last 32 bit = counter ops in second, https://docs.mongodb.com/manual/reference/bson-types/#timestamps
			{
				uint64_t timestamp;
				timestamp = ((uint64_t) data.get_timestamp().timestamp) << 32;
				timestamp |= (uint64_t) data.get_timestamp().increment;
				milk::bite bite(timestamp);
				bite.bin_extension(0x11); //http://bsonspec.org/spec.html
				return bite;
				break;
			}
			/*
			UNSUPPORTED AND DEPRECADED TYPES
			case bsoncxx::type::k_decimal128: // fp 128bit
				break;
			case bsoncxx::type::k_codewscope: // JS code with scope
				break;
			case bsoncxx::type::k_dbpointer: // deprecated
				break;
			case bsoncxx::type::k_symbol:
				break;
			case bsoncxx::type::k_undefined:
				break;
			case bsoncxx::type::k_maxkey: // mongo types
				break;
			case bsoncxx::type::k_minkey:
				break;
			*/

			};

			throw std::runtime_error("could not operate on element with given type! deprecated, mongoDB internal or not implemented!");
		};

		static milk::bite as_list(const bsoncxx::array::view& data)
		{
			milk::bite bite = std::vector<milk::bite>{};

			for (auto& it : data)
			{
				switch (it.type())
				{
				case bsoncxx::type::k_document: // is map
					bite.push_back(as_map(it.get_document().view()));
					break;
				case bsoncxx::type::k_array: // is list/array
					bite.push_back(as_list(it.get_array().value));
					break;
				default: // is scalar
					bite.push_back(as_scalar(it.get_value()));
					break;
				};
			};

			return bite;
		};

		// bson embeds all values in a document; no consecutive values without enclosing document!
		// arrays are documents/maps as well (string keys starting from 0 to n)
		static milk::bite as_map(const bsoncxx::document::view& data)
		{
			milk::bite bite = std::map<std::string, milk::bite>{};

			for (auto& it : data)
			{
				switch (it.type())
				{
				case bsoncxx::type::k_document: // is map
					bite.bite_ref_at(it.key().to_string()) = as_map(it.get_document().view());
					break;
				case bsoncxx::type::k_array: // is list/array
					bite.bite_ref_at(it.key().to_string()) = as_list(it.get_array().value);
					break;
				default: // is scalar
					bite.bite_ref_at(it.key().to_string()) = as_scalar(it.get_value());
					break;
				};
			};

			return bite;
		};


		static bsoncxx::builder::basic::array pack_list(milk::bite& data)
		{
			using bsoncxx::builder::basic::array;

			auto arr = array{};

			for (auto& it : data)
			{
				if (it.is_scalar())
				{
					// TODO: implement special bson types using binary extension

					// pack data
					switch (it.type())
					{
					case milk::t_type::s_int:
						switch (it.bin_extension())
						{
						case 0x09: // date
							arr.append(bsoncxx::types::b_date(std::chrono::milliseconds{ it.get<int64_t>() }));
							break;
						case 0x11: // timestamp
						{
							// first 32 bit = unix timestamp, last 32 bit = counter ops in second, https://docs.mongodb.com/manual/reference/bson-types/#timestamps
							uint64_t int_ts = it.get<uint64_t>();
							bsoncxx::types::b_timestamp b_ts;
							b_ts.timestamp = ((uint64_t)int_ts >> 32);
							b_ts.increment = int_ts | 0x00000000ffffffff;
							arr.append(b_ts);
							break;
						}
						default:
							arr.append(it.get<int>());
							break;
						}
						break;
					case milk::t_type::s_fp:
						arr.append(it.get<double>());
						break;
					case milk::t_type::s_byte:
						arr.append(it.get<unsigned char>());
						break;
					case milk::t_type::s_bool:
						arr.append(it.get<bool>());
						break;
					case milk::t_type::n_str:
						switch (it.bin_extension())
						{
						case 0x0b: // regex
							arr.append(bsoncxx::types::b_regex{ it.get<std::string>() });
							break;
						case 0x0d: // JS code
							arr.append(bsoncxx::types::b_code{ it.get<std::string>() });
							break;
						default:
							arr.append(it.get<std::string>());
							break;
						}
						break;
					case milk::t_type::n_bin:
						// copying binary is most inefficient way here, need access to data by pointer
						milk::binary_proxy bin = it.get<milk::binary_proxy>();

						// handle object id as special binary case
						if (it.bin_extension() == 0x07)
						{
							arr.append(bsoncxx::oid(reinterpret_cast<const char*>(bin.data()), bin.size()));
							break;
						}
						// else
						bsoncxx::types::b_binary bson_bin = { bsoncxx::binary_sub_type(it.bin_extension()), bin.size(), bin.data() };
						arr.append(bson_bin);
						break;
					}

					continue;
				}

				if (it.is_map())
				{
					arr.append(pack_map(it));
					continue;
				}

				if (it.is_list())
				{
					arr.append(pack_list(it));
					continue;
				}
			}

			return arr;

		};


		static bsoncxx::builder::basic::document pack_map(milk::bite& data)
		{
			using bsoncxx::builder::basic::kvp;
			using bsoncxx::builder::basic::document;

			auto doc = document{};

			for (auto& it : data)
			{
				if (it.is_scalar())
				{
					// TODO: implement special bson types using binary extension

					// pack data
					switch (it.type())
					{
					case milk::t_type::s_int:
						switch (it.bin_extension())
						{
						case 0x09: // date
							doc.append(kvp(it.first, bsoncxx::types::b_date(std::chrono::milliseconds{ it.get<int64_t>() })));
							break;
						case 0x11: // timestamp
						{
							// first 32 bit = unix timestamp, last 32 bit = counter ops in second, https://docs.mongodb.com/manual/reference/bson-types/#timestamps
							uint64_t int_ts = it.get<uint64_t>();
							bsoncxx::types::b_timestamp b_ts;
							b_ts.timestamp = ((uint64_t)int_ts >> 32);
							b_ts.increment = int_ts | 0x00000000ffffffff;
							doc.append(kvp(it.first, b_ts));
							break;
						}
						default:
							doc.append(kvp(it.first, it.get<int>()));
							break;
						}
						break;
					case milk::t_type::s_fp:
						doc.append(kvp(it.first, it.get<double>()));
						break;
					case milk::t_type::s_byte:
						doc.append(kvp(it.first, it.get<unsigned char>()));
						break;
					case milk::t_type::s_bool:
						doc.append(kvp(it.first, it.get<bool>()));
						break;
					case milk::t_type::n_str:
						switch (it.bin_extension())
						{
						case 0x0b: // regex
							doc.append(kvp(it.first, bsoncxx::types::b_regex{ it.get<std::string>() }));
							break;
						case 0x0d: // JS code
							doc.append(kvp(it.first, bsoncxx::types::b_code{ it.get<std::string>() }));
							break;
						default:
							doc.append(kvp(it.first, it.get<std::string>()));
							break;
						}
						break;
					case milk::t_type::n_bin:
						// copying binary is most inefficient way here, need access to data by pointer
						milk::binary_proxy bin = it.get<milk::binary_proxy>();

						// handle object id as special binary case
						if (it.bin_extension() == 0x07)
						{
							doc.append(kvp(it.first, bsoncxx::oid(reinterpret_cast<const char*>(bin.data()), bin.size())));
							break;
						}
						// else
						bsoncxx::types::b_binary bson_bin = { bsoncxx::binary_sub_type(it.bin_extension()), bin.size(), bin.data() };
						doc.append(kvp(it.first, bson_bin));
						break;

					}

					continue;
				}

				if (it.is_map())
				{
					doc.append(kvp(it.first, pack_map(it)));
					continue;
				}

				if (it.is_list())
				{
					doc.append(kvp(it.first, pack_list(it)));
					continue;
				}
			}

			return doc;

		};

	public:

		// non owning view
		static milk::bite from(const bsoncxx::document::view& data)
		{
			if (data.empty()) return milk::bite(); // empty document -> empty container (throw?)

			return as_map(data);
		};

		// from stream (pointer and size)
		static milk::bite from(const unsigned char* data, std::size_t size)
		{
			return from(bsoncxx::document::view(data, size));
		};

		// from owning bson document
		static milk::bite from(const bsoncxx::document::value& data)
		{
			return from(data.view());
		};

		static void to(milk::bite& data, bsoncxx::builder::basic::document& document)
		{
			if (!data.is_map())
				throw std::runtime_error("structure has no top-level map! BSON needs a top level map in the document!");

			document = pack_map(data);
		};

	};

}
