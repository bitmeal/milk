#include "grain.h"

namespace milk
{
	// DEFAULT
	grain::grain() : type(undefined), bin_ext(0) {};

	// COPY CONSTRUCTOR
	grain::grain(const grain& source) : grain()
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

	grain& grain::operator=(const grain& source)
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
	template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
	grain::grain(const T& val) : grain()
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
	grain::grain(const T& val) : grain()
	{
			type = s_fp;
			scalar_data.d_fp = val;
			return;
	};

	// map types
	template<typename T, std::enable_if_t<
		sizeof(typename T::key_type) != 0
	>* = nullptr >
	grain::grain(const T& val) : grain()
	{
		type = t_map;
		d_map = std::make_unique<map_t>(val.begin(), val.end());
	};

	// container types with iterators
	template<typename T, std::enable_if_t<
		!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
		!std::is_same<decltype(std::declval<T>().end()), void>::value &&
		!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value &&
		!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value
	>* = nullptr>
	grain::grain(const T& val) : grain()
	{
		type = t_list;
		d_list = std::make_unique<list_t>(val.begin(), val.end());
	};

	// container types with iterators and char data; interpreted as binary!
	template<typename T, std::enable_if_t<
		!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
		!std::is_same<decltype(std::declval<T>().end()), void>::value &&
		(std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value ||
		 std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value)
		>* = nullptr>
	grain::grain(const T& val) : grain()
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
	grain::grain(const T& val) : grain()
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

	// handling: milk::bite my_bite = "string/chararr";
	grain::grain(const char* &val) : grain(std::string(val)) { };

	// handling: char* ch = "chararr"; milk::bite my_bite = ch;
	template <typename T, std::enable_if_t<std::is_same<char*, std::remove_const_t<T>>::value>* = nullptr>
	grain::grain(const T& val) : grain(std::string(val)) { };

	template<>
	grain::grain(const std::string& val) : grain()
	{
		type = n_str;
		d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
	};

	template<>
	grain::grain(const bool& val) : grain()
	{
		type = s_bool;
		scalar_data.d_bool = val;
	};

	void grain::bin_extension(const unsigned char& val)
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

	grain::t_type grain::get_type() const { return type; }

	// CATCH INCOMPATIBLE CONVERSIONS AT COMPILE TIME!
	template <typename T>
	typename std::enable_if<!std::is_arithmetic<T>::value, T>::type
	grain::get() const
	{
		static_assert(false, "you tried to convert to an incompatible type!");
	}

	template <typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, T>::type
	grain::get() const
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

	template<>
	unsigned char grain::get<unsigned char>() const
	{
		if (type == s_byte)
			return scalar_data.d_byte;

		throw std::runtime_error("cannot convert to string");
	};

	template<>
	binary_proxy grain::get<binary_proxy>() const
	{

		if (type == n_bin || type == n_str)
		{
			return binary_proxy(d_str_bin->data(), d_str_bin->size());
		}

		switch (type) {
		case s_int:
			return binary_proxy((unsigned char*) &(scalar_data.d_int), sizeof(scalar_data.d_int));
			break;
		case s_fp:
			return binary_proxy((unsigned char*) &(scalar_data.d_fp), sizeof(scalar_data.d_fp));
			break;
		case s_byte:
			return binary_proxy((unsigned char*) &(scalar_data.d_byte), sizeof(scalar_data.d_byte));
			break;
		case s_bool:
			return binary_proxy((unsigned char*) &(scalar_data.d_bool), sizeof(scalar_data.d_bool));
			break;
		}

		throw std::runtime_error("cannot get raw unsigned char pointer and size of this type!");

	};



	template<>
	std::vector<unsigned char> grain::get<std::vector<unsigned char>>() const
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
	std::string grain::get<std::string>() const
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
	bool grain::get<bool>() const
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
	}

	unsigned char grain::bin_extension() const
	{
		return bin_ext;
	}

	bool grain::is_map() const
	{
		return (type == t_map);
	};

	bool grain::is_list() const
	{
		return (type == t_list);
	}

	bool grain::is_scalar() const
	{
		return (type != t_map && type != t_list);
	}

	bool grain::is_fp() const
	{
		return (type == s_fp);
	}

	bool grain::is_float() const { return is_fp(); }

	bool grain::is_int() const
	{
		return (type == s_int);
	}

	bool grain::is_bool() const
	{
		return (type == s_bool);
	}

	bool grain::is_byte() const
	{
		return (type == s_byte);
	}

	bool grain::is_str() const
	{
		return (type == n_str);
	}

	bool grain::is_bin() const
	{
		return (type == n_bin);
	}


	std::size_t grain::size() const
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

	const unsigned char* grain::data() const
	{
		if( d_str_bin && ( type == n_str || type == n_bin ))
			return d_str_bin->data();
		else
			return nullptr;
	}

	bite_iterator grain::begin(bite* parent)
	{
		switch (type)
		{
		case t_map:
			return bite_iterator(d_map->begin());
			break;
		case t_list:
			return bite_iterator(d_list->begin());
			break;
		default:
			return bite_iterator(parent);
		}
	}

	bite_iterator grain::end(bite* parent)
	{
		switch (type)
		{
		case t_map:
			return bite_iterator(d_map->end());
			break;
		case t_list:
			return bite_iterator(d_list->end());
			break;
		default:
			return bite_iterator(parent + 1);
		}
	}

	bite_iterator grain::find(const std::string& key, bite* parent)
	{
		if (type == t_map)
			return bite_iterator(d_map->find(key));
		else
			return end(parent);
	}

	// throws out of range exception or runtime error when called on non container grain
	bite& grain::idx(const std::size_t idx)
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

	bite& grain::back()
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

	bite& grain::front()
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

	bool grain::idx_probe(const std::string& key)
	{
		if (type != t_map) return false;

		return (d_map->find(key) != d_map->end());
	}

	bite& grain::idx(const std::string& key)
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
	void grain::push_back(const T& val)
	{
		if (type != t_list)
			throw std::runtime_error("bite is no list; cannot use push_back here! implicit conversion from scalar to list should have already taken place if called through bite interface");

		d_list->push_back(bite(val));
	}

	void grain::pop_back()
	{
		if (type == t_list)
			d_list->pop_back();
	}

	std::size_t grain::erase(const std::string& key)
	{
		if (type != t_map)
			return 0;

		return d_map->erase(key);
	}

	bite_iterator grain::erase(bite_iterator first, bite_iterator last, bite* parent)
	{
		// erase
		switch (type)
		{
		case t_map:
			if (first.iterator_type == bite_iterator::iterator_type_t::map_i && last.iterator_type == bite_iterator::iterator_type_t::map_i)
				return bite_iterator(d_map->erase(first.map_iterator, last.map_iterator));
			else
				throw std::runtime_error("iterators for erase operation of wrong type!");
			break;
		case t_list:
			if (first.iterator_type == bite_iterator::iterator_type_t::list_i && last.iterator_type == bite_iterator::iterator_type_t::list_i)
				return bite_iterator(d_list->erase(first.list_iterator, last.list_iterator));
			else
				throw std::runtime_error("iterators for erase operation of wrong type!");
			break;
		}
	}


	grain::~grain() {};

}
