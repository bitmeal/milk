#pragma once

namespace milk
{
	template<typename B>
	class grain_base
	{
		public:
			typedef std::vector<unsigned char>	str_bin_t;
			typedef std::map<std::string, B>	map_t;
			typedef std::vector<B>				list_t;

		private:
			union t_scalar_data {
				int64_t			d_int;
				double			d_fp;
				unsigned char	d_byte;
				bool			d_bool;
			};
			t_scalar_data scalar_data;
			milk::t_type type;

			unsigned char bin_ext;

			int l_str_bin;
			std::unique_ptr<str_bin_t>	d_str_bin;
			std::unique_ptr<map_t>		d_map;
			std::unique_ptr<list_t>		d_list;


		public:
			// DEFAULT
			grain_base() : type(undefined), bin_ext(0) {};

			// COPY CONSTRUCTOR
			grain_base(const grain_base<B>& source) : grain_base()
			{
				type = source.type;
				scalar_data = source.scalar_data;
				bin_ext = source.bin_ext;

				switch (type)
				{
				case t_map:
					d_map = std::make_unique<map_t>(*(source.d_map.get()));
					break;
				case t_list:
					d_list = std::make_unique<list_t>(*(source.d_list.get()));
					break;
				case n_str:
				case n_bin:
					d_str_bin = std::make_unique<str_bin_t>(*(source.d_str_bin.get()));
					break;
				}
			};

			grain_base<B>& operator=(const grain_base<B>& source)
			{
				type = source.type;
				scalar_data = source.scalar_data;
				bin_ext = source.bin_ext;

				switch (type)
				{
				case t_map:
					d_map = std::make_unique<map_t>(*(source.d_map.get()));
					break;
				case t_list:
					d_list = std::make_unique<list_t>(*(source.d_list.get()));
					break;
				case n_str:
				case n_bin:
					d_str_bin = std::make_unique<str_bin_t>(*(source.d_str_bin.get()));
					break;
				}

				return *this;
			}

			/*
			CONSTRUCTORS - SETTERS
			 - integral types + char/uchar
			 - floating point types
			 - std::string
			 - string from char*
			 - binary from iterable char/uchar container
			 - binary from object with size_t size() and char* data()
			 - iterable container
			 - std::map<std::string, <T>>
			 - bool
			*/

			// integral types (bool has specialization - why?)
			template <typename T, std::enable_if_t<
				std::is_integral<T>::value &&
				!std::is_same<T, bool>::value
				>* = nullptr>
			grain_base(const T& val) : grain_base()
			{
				if (typeid(T) == typeid(char) || typeid(T) == typeid(unsigned char))
				{
					type = s_byte;
					scalar_data.d_byte = val;
					return;
				}

				type = s_int;
				scalar_data.d_int = val;
				return;
			};

			// floating point types
			template <typename T, std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
			grain_base(const T& val) : grain_base()
			{
					type = s_fp;
					scalar_data.d_fp = val;
					return;
			};

			// map types
			template<typename T, std::enable_if_t<
				sizeof(typename T::key_type) != 0
			>* = nullptr >
				grain_base(const T& val) : grain_base()
			{
				type = t_map;
				d_map = std::make_unique<map_t>(val.begin(), val.end());
			};

			// container types with iterators
			template<typename T, std::enable_if_t<
				!std::is_same<std::string, T>::value &&
				!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
				!std::is_same<decltype(std::declval<T>().end()), void>::value &&
				!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value &&
				!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value
			>* = nullptr>
				grain_base(const T& val) : grain_base()
			{
				type = t_list;
				d_list = std::make_unique<list_t>(val.begin(), val.end());
			};

			// container types with iterators and char data; interpreted as binary!
			template<typename T, std::enable_if_t<
				!std::is_same<std::string, T>::value &&
				!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
				!std::is_same<decltype(std::declval<T>().end()), void>::value &&
				(std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value ||
				 std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value)
				>* = nullptr>
				grain_base(const T& val) : grain_base()
			{
					type = n_bin;
					d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
			};

			// has size_t size() and char* data()
			template<typename T, std::enable_if_t<
				!std::is_same<std::string, T>::value &&
				std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
				(std::is_same<const char*, decltype(std::declval<T>().data())>::value ||
				std::is_same<const unsigned char*, decltype(std::declval<T>().data())>::value)
			>* = nullptr>
			grain_base(const T& val) : grain_base()
			{
				type = n_bin;
				d_str_bin = std::make_unique<str_bin_t>(val.data(), val.data() + val.size());
				/*
				d_str_bin = std::make_unique<str_bin_t>();
				d_str_bin->reserve(val.size());
				for (std::size_t idx = 0; idx < val.size(); ++idx)
					d_str_bin->push_back(*(val.data() + idx));
				*/
			};

			template <typename T, std::enable_if_t<std::is_same<std::string, T>::value>* = nullptr>
			grain_base(const T& val) : grain_base()
			{
				type = n_str;
				d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
			};

			template <typename T, std::enable_if_t<std::is_same<bool, T>::value>* = nullptr>
			grain_base(const T& val) : grain_base()
			{
				type = s_bool;
				scalar_data.d_bool = val;
			};


			// handling: milk::bite my_bite = "string/chararr";
			grain_base(const char* &val) : grain_base(std::string(val)) { };

			// handling: char* ch = "chararr"; milk::bite my_bite = ch;
			template <typename T, std::enable_if_t<std::is_same<char*, std::remove_const_t<T>>::value>* = nullptr>
			grain_base(const T& val) : grain_base(std::string(val)) { };


			void bin_extension(const unsigned char& val)
			{
				bin_ext = val;
			}

			/*
			GETTERS
			attempting auto conversion here. throws if not possible!
			 - arithmetic types
			 - uchar
			 - uchar vector (binary), conversion from all types
			 - std::string
			 - bool; conversion from all types with: false if value or size of held type = 0, true otherwise

			*/

			milk::t_type get_type() const { return type; }

			// CATCH INCOMPATIBLE CONVERSIONS AT COMPILE TIME!
/*
			template <typename T>
			typename std::enable_if<
				!std::is_same<bool, T>::value
				&& !std::is_same<std::string, T>::value
				&& !std::is_same<std::vector<unsigned char>, T>::value
				&& !std::is_same<milk::binary_proxy, T>::value
				&& !std::is_same<unsigned char, T>::value
				&& !std::is_arithmetic<T>::value
			, T>::type
			get() const
			{
				static_assert(false, "you tried to convert to an incompatible type!");
			}
*/
			template <typename T>
			typename std::enable_if<
				std::is_arithmetic<T>::value
				&& !std::is_same<unsigned char, T>::value
				&& !std::is_same<bool, T>::value
			, T>::type
			get() const
			{
				switch (type) {
				case s_int:
					return static_cast<T>(scalar_data.d_int);
					break;
				case s_fp:
					return static_cast<T>(scalar_data.d_fp);
					break;
				}

				throw std::runtime_error("cannot convert to arithmetic");

			};


			template<typename T>
			typename std::enable_if<std::is_same<unsigned char, T>::value, T>::type
			get() const
			{
				if (type == s_byte)
					return scalar_data.d_byte;

				throw std::runtime_error("cannot convert to string");
			};

			template<typename T>
			typename std::enable_if<std::is_same<milk::binary_proxy, T>::value, T>::type
			get() const
			{

				if (type == n_bin || type == n_str)
				{
					return milk::binary_proxy(d_str_bin->data(), d_str_bin->size());
				}

				switch (type) {
				case s_int:
					return milk::binary_proxy((unsigned char*) &(scalar_data.d_int), sizeof(scalar_data.d_int));
					break;
				case s_fp:
					return milk::binary_proxy((unsigned char*) &(scalar_data.d_fp), sizeof(scalar_data.d_fp));
					break;
				case s_byte:
					return milk::binary_proxy((unsigned char*) &(scalar_data.d_byte), sizeof(scalar_data.d_byte));
					break;
				case s_bool:
					return milk::binary_proxy((unsigned char*) &(scalar_data.d_bool), sizeof(scalar_data.d_bool));
					break;
				}

				throw std::runtime_error("cannot get raw unsigned char pointer and size of this type!");

			};

			template<typename T>
			typename std::enable_if<std::is_same<std::vector<unsigned char>, T>::value, T>::type
			get() const
			{
				if (type == n_bin || type == n_str)
				{
					return *(d_str_bin.get());
				}

				std::vector<unsigned char> ch_vec;

				switch (type) {
				case s_int:
					for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
						ch_vec.push_back(*((char*) &(scalar_data.d_int) + i));
					return ch_vec;
					break;
				case s_fp:
					for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
						ch_vec.push_back(*((char*) &(scalar_data.d_fp) + i));
					return ch_vec;
					break;
				case s_byte:
					ch_vec.push_back(*((char*) &(scalar_data.d_byte)));
					return ch_vec;
					break;
				case s_bool:
					ch_vec.push_back(*((char*) &(scalar_data.d_bool)));
					return ch_vec;
					break;
				}

				throw std::runtime_error("cannot convert to char vector");

			};

			template<typename T>
			typename std::enable_if<std::is_same<std::string, T>::value, T>::type
			get() const
			{
				switch (type) {
				case s_int:
					return std::to_string(scalar_data.d_int);
					break;
				case s_fp:
					return std::to_string(scalar_data.d_fp);
					break;
				case s_byte:
					return std::to_string(scalar_data.d_byte);
					break;
				case s_bool:
					return std::to_string(scalar_data.d_bool);
					break;
				case n_str:
				case n_bin:
					return std::string(d_str_bin->begin(), d_str_bin->end());
					break;
				}

				//convert map and list to empty string?
				throw std::runtime_error("cannot convert to string");

			};

			template<typename T>
			typename std::enable_if<std::is_same<bool, T>::value, T>::type
			get() const
			{
				switch (type) {
				case s_bool:
					return scalar_data.d_bool;
					break;
				case s_int:
					return (scalar_data.d_int != 0);
					break;
				case s_fp:
					return (scalar_data.d_fp != 0);
					break;
				case s_byte:
					return (scalar_data.d_byte != 0);
					break;
				case n_str:
				case n_bin:
					return (d_str_bin->size() != 0);
					break;
				case t_list:
					return (d_list->size() != 0);
					break;
				case t_map:
					return (d_map->size() != 0);
					break;
				default:
					return false;
				}
			};





			unsigned char bin_extension() const
			{
				return bin_ext;
			}

			bool is_map() const
			{
				return (type == t_map);
			};

			bool is_list() const
			{
				return (type == t_list);
			}

			bool is_scalar() const
			{
				return (type != t_map && type != t_list);
			}

			bool is_fp() const
			{
				return (type == s_fp);
			}

			bool is_float() { return is_fp(); }

			bool is_int() const
			{
				return (type == s_int);
			}

			bool is_bool() const
			{
				return (type == s_bool);
			}

			bool is_byte() const
			{
				return (type == s_byte);
			}

			bool is_str() const
			{
				return (type == n_str);
			}

			bool is_bin() const
			{
				return (type == n_bin);
			}


			std::size_t size() const
			{
				switch (type)
				{
				case t_map:
					return d_map->size();
					break;
				case t_list:
					return d_list->size();
					break;
				case n_str:
				case n_bin:
					return d_str_bin->size();
				default:
					return 1;
				}
			}

			const unsigned char* data() const
			{
				if( d_str_bin && ( type == n_str || type == n_bin ))
					return d_str_bin->data();
				else
					return nullptr;
			}

			milk::bite_iterator_base<B, milk::grain_base<B>> begin(B* parent)
			{
				switch (type)
				{
				case t_map:
					return milk::bite_iterator_base<B, milk::grain_base<B>>(d_map->begin());
					break;
				case t_list:
					return milk::bite_iterator_base<B, milk::grain_base<B>>(d_list->begin());
					break;
				default:
					return milk::bite_iterator_base<B, milk::grain_base<B>>(parent);
				}
			}

			milk::bite_iterator_base<B, milk::grain_base<B>> end(B* parent)
			{
				switch (type)
				{
				case t_map:
					return milk::bite_iterator(d_map->end());
					break;
				case t_list:
					return milk::bite_iterator(d_list->end());
					break;
				default:
					return milk::bite_iterator(parent + 1);
				}
			}

			milk::bite_iterator_base<B, milk::grain_base<B>> find(const std::string& key, B* parent)
			{
				if (type == t_map)
					return milk::bite_iterator(d_map->find(key));
				else
					return end(parent);
			}

			// throws out of range exception or runtime error when called on non container grain
			B& idx(const std::size_t idx)
			{
				typename map_t::iterator map_idx_it;
				switch (type)
				{
				case t_map:
					map_idx_it = d_map->begin();
					std::advance(map_idx_it, idx);
					return map_idx_it->second;
					break;
				case t_list:
					return d_list->at(idx);
					break;
				default:
					throw std::runtime_error("bite is no container type! no index access possible!");
				}
			}

			B& back()
			{
				// if is scalar, back is handled in bite as interface to grain
				// if grain contains a map, back will be the last iterable element in the map!
				typename map_t::iterator map_idx_it;
				switch (type)
				{
				case t_map:
					map_idx_it = d_map->begin();
					std::advance(map_idx_it, d_map->size() - 1);
					return map_idx_it->second;
					break;
				case t_list:
					return d_list->back();
					break;
				default:
					throw std::runtime_error("bite is no container type! no index access possible!");
				}
			}

			B& front()
			{
				// if is scalar, back is handled in bite as interface to grain
				// if grain contains a map, front will be the forst iterable element in the map!
				typename map_t::iterator map_idx_it;
				switch (type)
				{
				case t_map:
					map_idx_it = d_map->begin();
					return map_idx_it->second;
					break;
				case t_list:
					return d_list->front();
					break;
				default:
					throw std::runtime_error("bite is no container type! no index access possible!");
				}
			}

			bool idx_probe(const std::string& key)
			{
				if (type != t_map) return false;

				return (d_map->find(key) != d_map->end());
			}

			B& idx(const std::string& key)
			{
				if (type != t_map)
				{
					d_list.release();
					d_str_bin.release();

					type = t_map;

					d_map = std::make_unique<map_t>();
				}

				return (*d_map)[key];
			}

			template<typename T>
			void push_back(const T& val)
			{
				if (type != t_list)
					throw std::runtime_error("bite is no list; cannot use push_back here! implicit conversion from scalar to list should have already taken place if called through bite interface");

				d_list->push_back(B(val));
			}

			void pop_back()
			{
				if (type == t_list)
					d_list->pop_back();
			}

			std::size_t erase(const std::string& key)
			{
				if (type != t_map)
					return 0;

				return d_map->erase(key);
			}

			milk::bite_iterator_base<B, milk::grain_base<B>> erase(milk::bite_iterator_base<B, milk::grain_base<B>> first, milk::bite_iterator_base<B, milk::grain_base<B>> last, B* parent)
			{
				// erase
				switch (type)
				{
				case t_map:
					if (first.iterator_type == milk::iterator_type_t::map_i && last.iterator_type == milk::iterator_type_t::map_i)
						return milk::bite_iterator(d_map->erase(first.map_iterator, last.map_iterator));
					else
						throw std::runtime_error("iterators for erase operation of wrong type!");
					break;
				case t_list:
					if (first.iterator_type == milk::iterator_type_t::list_i && last.iterator_type == milk::iterator_type_t::list_i)
						return milk::bite_iterator(d_list->erase(first.list_iterator, last.list_iterator));
					else
						throw std::runtime_error("iterators for erase operation of wrong type!");
					break;
				}
			}


			~grain_base() {};
	};

	/*
	template<>
	template<>
	inline unsigned char grain_base<milk::bite>::get<unsigned char>() const
	{
		if (type == s_byte)
			return scalar_data.d_byte;

		throw std::runtime_error("cannot convert to string");
	};

	template<>
	template<>
	inline milk::binary_proxy grain_base<milk::bite>::get<milk::binary_proxy>() const
	{

		if (type == n_bin || type == n_str)
		{
			return milk::binary_proxy(d_str_bin->data(), d_str_bin->size());
		}

		switch (type) {
		case s_int:
			return milk::binary_proxy((unsigned char*) &(scalar_data.d_int), sizeof(scalar_data.d_int));
			break;
		case s_fp:
			return milk::binary_proxy((unsigned char*) &(scalar_data.d_fp), sizeof(scalar_data.d_fp));
			break;
		case s_byte:
			return milk::binary_proxy((unsigned char*) &(scalar_data.d_byte), sizeof(scalar_data.d_byte));
			break;
		case s_bool:
			return milk::binary_proxy((unsigned char*) &(scalar_data.d_bool), sizeof(scalar_data.d_bool));
			break;
		}

		throw std::runtime_error("cannot get raw unsigned char pointer and size of this type!");

	};

	template<>
	template<>
	inline std::vector<unsigned char> grain_base<milk::bite>::get<std::vector<unsigned char>>() const
	{
		if (type == n_bin || type == n_str)
		{
			return *(d_str_bin.get());
		}

		std::vector<unsigned char> ch_vec;

		switch (type) {
		case s_int:
			for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
				ch_vec.push_back(*((char*) &(scalar_data.d_int) + i));
			return ch_vec;
			break;
		case s_fp:
			for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
				ch_vec.push_back(*((char*) &(scalar_data.d_fp) + i));
			return ch_vec;
			break;
		case s_byte:
			ch_vec.push_back(*((char*) &(scalar_data.d_byte)));
			return ch_vec;
			break;
		case s_bool:
			ch_vec.push_back(*((char*) &(scalar_data.d_bool)));
			return ch_vec;
			break;
		}

		throw std::runtime_error("cannot convert to char vector");

	};

	template<>
	template<>
	inline std::string grain_base<milk::bite>::get<std::string>() const
	{
		switch (type) {
		case s_int:
			return std::to_string(scalar_data.d_int);
			break;
		case s_fp:
			return std::to_string(scalar_data.d_fp);
			break;
		case s_byte:
			return std::to_string(scalar_data.d_byte);
			break;
		case s_bool:
			return std::to_string(scalar_data.d_bool);
			break;
		case n_str:
		case n_bin:
			return std::string(d_str_bin->begin(), d_str_bin->end());
			break;
		}

		//convert map and list to empty string?
		throw std::runtime_error("cannot convert to string");

	};

	template<>
	template<>
	inline bool grain_base<milk::bite>::get<bool>() const
	{
		switch (type) {
		case s_bool:
			return scalar_data.d_bool;
			break;
		case s_int:
			return (scalar_data.d_int != 0);
			break;
		case s_fp:
			return (scalar_data.d_fp != 0);
			break;
		case s_byte:
			return (scalar_data.d_byte != 0);
			break;
		case n_str:
		case n_bin:
			return (d_str_bin->size() != 0);
			break;
		case t_list:
			return (d_list->size() != 0);
			break;
		case t_map:
			return (d_map->size() != 0);
			break;
		default:
			return false;
		}
	};
	*/

}
