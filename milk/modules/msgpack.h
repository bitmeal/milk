
#include <utility>
#include <functional>
#include <forward_list>
#include <string>

#include <msgpack.hpp>


namespace milk
{
	struct msgpack_module {
	private:
		struct milk_msgpack_visitor : public msgpack::v2::null_visitor {
		private:
			milk::bite& data;

			enum t_structure_state { uninit, map, list, ret_toplevel };
			t_structure_state structure_state;

			enum t_data_state { key, value };
			t_data_state data_state;

			std::forward_list< std::pair<
				milk::bite&,
				t_structure_state
			> > depth_manager;

			std::string key_cache;

			bool implicit_string_keys;

		public:
			milk_msgpack_visitor() = delete;
			milk_msgpack_visitor(milk::bite& data) : data(data), structure_state(uninit), key_cache(""), data_state(value), implicit_string_keys(true) {};
			milk_msgpack_visitor(milk::bite& data, bool implicit_string_keys) : milk_msgpack_visitor(data) { this->implicit_string_keys = implicit_string_keys; };

			struct binary_wrapper
			{
				const unsigned char* _data;
				std::size_t _size;

				binary_wrapper(const unsigned char* _data, std::size_t _size) : _data(_data), _size(_size) {};

				const unsigned char* data() const { return _data; };
				std::size_t size() const { return _size; };
			};

		private:
			template <typename T>
			inline bool process_scalar(T& v)
			{
				// simple statemachine
				switch (structure_state)
				{
				case uninit:
					// msgpack can pack values in a stream without declaring it as a list/array - the stream ist the implicit to level list; we need to pack these as a list!
					data = v;
					structure_state = list;
					depth_manager.push_front(
						std::make_pair(std::ref(data), structure_state)
					);
					break;
				case list:
					depth_manager.front().first.push_back(v);
					break;
				case map:
					switch (data_state)
					{
					case key:
						if (!implicit_string_keys) return false; //no implicit conversion of non-string keys to string permitted!
						else key_cache = std::to_string(v);
						break;
					case value:
						depth_manager.front().first[key_cache] = v;
						break;
					};
				};

				return true;
			}

			bool process_structure(t_structure_state state)
			{

				if (structure_state == ret_toplevel) process_toplevel_return();

				switch (structure_state)
				{
				case uninit:
					depth_manager.push_front(
						std::make_pair(std::ref(data), state)
					);
					break;
				case list:
					depth_manager.front().first.push_back(milk::bite());
					depth_manager.push_front(std::make_pair(std::ref(depth_manager.front().first.back()), state));
					break;
				case map:
					switch (data_state)
					{
					case key:
						return false; //array/list as map key not possible!
						break;
					case value:
						depth_manager.push_front(std::make_pair(std::ref(depth_manager.front().first.bite_ref_at(key_cache)), state));
						break;
					};

				};
				
				switch (state)
				{
				case map:
					depth_manager.front().first = std::map<std::string, milk::bite>{};
					break;
				case list:
					 depth_manager.front().first = std::vector<milk::bite>{};
					break;
				}
				
				structure_state = state;

				return true;
			}

			// called when first element to fetch from messagepack-stream was a map or list/array and we need do pack these into a list to keep adding top-level elements
			void process_toplevel_return()
			{
				std::vector<milk::bite> swap;
				swap.push_back(data);
				data = swap;

				depth_manager.push_front(
					std::make_pair(std::ref(data), list)
				);

				structure_state = list;
			}

			void depth_pop()
			{
				depth_manager.pop_front();
				if (!depth_manager.empty())
					structure_state = depth_manager.front().second;
				else
					structure_state = ret_toplevel;

				// we just "closed" our top-level container
				// happens only if first element was a map or list/array

				// introduce ret_toplevel status -> helper function setting depth_manager and status to list

			}

		public:
			bool visit_nil() {
				return false; //cannot handle null-type
			}
			bool visit_positive_integer(uint64_t v) {
				return process_scalar(v);
			}
			bool visit_negative_integer(int64_t v) {
				return process_scalar(v);
			}
			bool visit_float32(float v) {
				return process_scalar(v);
			}
			bool visit_float64(double v) {
				return process_scalar(v);
			}
			bool visit_str(const char* ptr, uint32_t size) {

				if (structure_state == ret_toplevel) process_toplevel_return();

				std::string v(ptr, size);

				switch (structure_state)
				{
				case uninit:
					// msgpack can pack values in a stream without declaring it as a list/array - the stream ist the implicit to level list; we need to pack these as a list!
					data = v;
					structure_state = list;
					depth_manager.push_front(
						std::make_pair(std::ref(data), structure_state)
					);
					break;
				case list:
					depth_manager.front().first.push_back(v);
					break;
				case map:
					switch (data_state)
					{
					case key:
						key_cache = v;
						break;
					case value:
						depth_manager.front().first[key_cache] = v;
						break;
					};
				};

				return true;
			}
			bool visit_bin(const char* ptr, uint32_t size) {

				if (structure_state == ret_toplevel) process_toplevel_return();

				binary_wrapper v = binary_wrapper(reinterpret_cast<const unsigned char*>(ptr), static_cast<std::size_t>(size));

				switch (structure_state)
				{
				case uninit:
					// msgpack can pack values in a stream without declaring it as a list/array - the stream ist the implicit to level list; we need to pack these as a list!
					data = v;
					depth_manager.push_front(
						std::make_pair(std::ref(data), structure_state)
					);
					structure_state = list;
					break;
				case list:
					depth_manager.front().first.push_back(v);
					break;
				case map:
					switch (data_state)
					{
					case key:
						if (!implicit_string_keys) return false; //no implicit conversion of non-string keys to string permitted!
						else key_cache = std::string(ptr, size);
						break;
					case value:
						depth_manager.front().first[key_cache] = v;
						break;
					};
				};

				return true;

			}
			bool visit_ext(const char* ptr, uint32_t size) {

				if (structure_state == ret_toplevel) process_toplevel_return();

				// type = *ptr; data = *(ptr + 1) with len = size - 1
				binary_wrapper bin = binary_wrapper(reinterpret_cast<const unsigned char*>(ptr + 1), static_cast<std::size_t>(size - 1));
				char ext = *(ptr);

				milk::bite v(bin);
				v.bin_extension(ext);

				switch (structure_state)
				{
				case uninit:
					// msgpack can pack values in a stream without declaring it as a list/array - the stream ist the implicit to level list; we need to pack these as a list!
					data = v;
					depth_manager.push_front(
						std::make_pair(std::ref(data), structure_state)
					);
					structure_state = list;
					break;
				case list:
					depth_manager.front().first.push_back(v);
					break;
				case map:
					switch (data_state)
					{
					case key:
						if (!implicit_string_keys) return false; //no implicit conversion of non-string keys to string permitted!
						else key_cache = v.get<std::string>();
						break;
					case value:
						depth_manager.front().first[key_cache] = v;
						break;
					};
				};

				return true;
			}

			bool start_array(uint32_t) {
				if (structure_state == ret_toplevel) process_toplevel_return();

				return process_structure(list);
			}

			bool end_array() {
				depth_pop();
				return true;
			}

			bool start_map(uint32_t) {
				if (structure_state == ret_toplevel) process_toplevel_return();

				return process_structure(map);
			}

			bool start_map_key() {
				data_state = key;
				return true;
			}

			bool start_map_value() {
				data_state = value;
				return true;
			}

			bool end_map() {
				depth_pop();
				return true;
			}
		};

		
		template <typename T>
		static void to_msgpack(milk::bite& data, msgpack::packer<T>& msgpack_packer)
		{
			if (data.is_scalar())
			{
				// pack data
				milk::type type = data.type();

				switch (type)
				{
				case milk::type::s_int:
					msgpack_packer.pack_int(data.get<int>());
					break;
				case milk::type::s_fp:
					msgpack_packer.pack_double(data.get<double>());
					break;
				case milk::type::s_byte:
					msgpack_packer.pack_unsigned_char(data.get<unsigned char>());
					break;
				case milk::type::s_bool:
					if (data.get<bool>())
						msgpack_packer.pack_true();
					else
						msgpack_packer.pack_false();
					break;
				case milk::type::n_str:
					msgpack_packer.pack(data.get<std::string>());
					break;
				case milk::type::n_bin:
					// copying binary is most inefficient way here, need access to data by pointer
					milk::binary_proxy bin = data.get<milk::binary_proxy>();
					if (data.bin_extension() == 0)
					{
						msgpack_packer.pack_bin(bin.size());
						msgpack_packer.pack_bin_body(reinterpret_cast<const char*>(bin.data()), bin.size());
					}
					else
					{
						msgpack_packer.pack_ext(bin.size(), data.bin_extension());
						msgpack_packer.pack_ext_body(reinterpret_cast<const char*>(bin.data()), bin.size());
					}
					break;
				}
				return;
			}

			if (data.is_map())
			{
				// pack map header
				msgpack_packer.pack_map(data.size());
				// iterate map
				for (auto& it : data)
				{
					// pack key
					msgpack_packer.pack((std::string) it.first);
					// recurse on value
					to_msgpack(it, msgpack_packer);
				}
				return;
			}

			if (data.is_list())
			{
				// pack list header
				msgpack_packer.pack_array(data.size());
				// iterate list
				for (auto& it : data)
				{
					//recurse on value
					to_msgpack(it, msgpack_packer);
				}
			}
		}

	public:
		static void from(const char* data, std::size_t size, milk::bite& m_bite, bool single_element = false)
		{
			milk::msgpack_module::milk_msgpack_visitor milk_visitor(m_bite);
			std::size_t offset = 0;

			if (!msgpack::v2::parse(data, size, offset, milk_visitor))
				m_bite = milk::bite();

			while (offset < size && !single_element)
			{
				if (!msgpack::v2::parse(data, size, offset, milk_visitor))
					m_bite = milk::bite();
			}
		}

		static std::shared_ptr<milk::bite> sp_from(const char* data, std::size_t size, bool single_element = false)
		{
			std::shared_ptr<milk::bite> bite_sp = std::make_shared<milk::bite>();
			from(data, size, *bite_sp, single_element);
			return bite_sp;
		}

		static milk::bite from(const char* data, std::size_t size, bool single_element = false)
		{
			milk::bite m_bite;
			from(data, size, m_bite, single_element);
			return m_bite;
		}

		template <typename T>
		static void to(milk::bite& data, T& buffer, bool pack_top_level_list = false)
		{
			msgpack::packer<T> msgpack_packer(buffer);
			

			if (data.is_map())
			{
				to_msgpack<T>(data, msgpack_packer);
				return;
			}

			// we need a top level bite-list to hold multiple maps, lists or scalar values, a stream can server as this container by itself and write top-level data directly to the stream
			if (pack_top_level_list && data.is_list())
				msgpack_packer.pack_array(data.size());


			// handles lists and scalars
			for (auto& it : data)
				to_msgpack<T>(it, msgpack_packer);
		}
	};
}