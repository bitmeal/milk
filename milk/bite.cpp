#include "bite.h"

namespace milk
{
	bite::bite() : grain(nullptr) { };

	bite::bite(const milk::grain& source)
	{
		//grain.release();
		grain = std::make_shared<milk::grain>(source);
	}

	bite::bite(const bite& source)
	{
		//grain.release();
		if(source.grain)
			grain = std::make_shared<milk::grain>(*(source.grain.get()));

	};

	template<typename T, std::enable_if_t<
		!std::is_same<bite, T>::value &&
		!std::is_same<bite_member_proxy, T>::value
	>* = nullptr>
		bite::bite(T& val)
	{
		set(val);
	};

	template<typename T>
	void bite::set(const T val) {
		if(!grain)
			grain = std::make_shared<grain>(val);
		else
			*grain = grain(val);
	};

	void bite::bin_extension(const unsigned char& val)
	{
		grain->bin_extension(val);
	}

	template<typename T>
	T bite::get() const {
		return grain ? grain->get<T>() : T();
	};

	unsigned char bite::bin_extension() const
	{
		return grain->bin_extension();
	}

	milk::type bite::type() const { return grain->get_type(); }


	bool bite::is_map() 	const { return grain ? 	grain->is_map()		: false;}
	bool bite::is_list() 	const { return grain ? 	grain->is_list()	: false;}
	bool bite::is_scalar()	const { return grain ? 	grain->is_scalar()	: false;}

	bool bite::is_fp() 		const { return grain ? 	grain->is_fp()		: false;}
	bool bite::is_float() 	const { return 			is_fp(); 					}
	bool bite::is_int() 	const { return grain ? 	grain->is_int()		: false;}
	bool bite::is_bool() 	const { return grain ? 	grain->is_bool()	: false;}
	bool bite::is_byte() 	const { return grain ? 	grain->is_byte()	: false;}
	bool bite::is_str() 	const { return grain ? 	grain->is_str()		: false;}
	bool bite::is_string() 	const { return 			is_str(); 					}
	bool bite::is_bin() 	const { return grain ? 	grain->is_bin()		: false;}
	bool bite::is_binary() 	const { return 			is_bin(); 					}


	std::size_t bite::size() const { return grain ? grain->size() : 0; };
	bool bite::empty() const { return (size() == 0); }

	const unsigned char* bite::data() const { return grain ? grain->data() : nullptr; };

	bite_iterator bite::begin()
	{
		return grain->begin(this);
	}

	bite_iterator bite::end()
	{
		return grain->end(this);
	}

	bite_iterator bite::find(const std::string& key)
	{
		return grain->find(key, this);
	}

	bite bite::at(const std::size_t idx) const
	{
		return grain->is_scalar() ? *this : grain->idx(idx);
	};

	bite& bite::at(const std::size_t idx)
	{
		return grain->is_scalar() ? *this : grain->idx(idx);
	};

	bite bite::operator [](const std::size_t idx) const {	return at(idx);	};

	bite& bite::operator [](const std::size_t idx) { return at(idx); };

	bite& bite::back() { return grain->is_scalar() ? *this : grain->back(); }
	bite& bite::front() { return grain->is_scalar() ? *this : grain->front(); }

	bite_member_proxy bite::at(const std::string& key)
	{
		return bite_member_proxy(key, *this, grain ? ( grain->idx_probe(key) ? grain->idx(key).grain : std::shared_ptr<milk::grain>(nullptr) ) : std::shared_ptr<milk::grain>(nullptr) );
	};

	bite bite::at(const std::string& key) const
	{
		return grain ? ( grain->idx_probe(key) ? grain->idx(key) : bite() ) : bite();
	};

	bite& bite::bite_ref_at(const std::string& key)
	{
		//makes this instance a map, not matter what! and inserts the requested key!
		if(!grain)
			grain = std::make_shared<grain>();

		return grain->idx(key);
	};

	bite_member_proxy bite::operator [](const std::string& key) { return at(key); };

	bite bite::operator [](const std::string& key) const { return at(key); };


	template<typename T>
	void bite::push_back(T val)
	{
		std::vector<bite> bite_vec;

		// instantiate new
		if (!grain)
		{
			bite_vec.push_back(bite(val));
			set(bite_vec);
			return;
		}

		// implicit conversion from scalar
		if (is_scalar())
		{
			bite_vec.push_back(*this);
			bite_vec.push_back(bite(val));
			set(bite_vec);
			return;
		}

		// leave handling to grain
		grain->push_back<T>(val);
	};

	void bite::pop_back()
	{
		grain->pop_back();
	}

	std::size_t bite::erase(const std::string& key)
	{
		return grain->erase(key);
	}

	bite_iterator bite::erase(bite_iterator first, bite_iterator last)
	{
		// range = 0
		if (first == last)
		{
			if (last == end())
				return last;
			else
				return ++last;
		}

		if (is_scalar())
		{
			if (first == begin() && last == end() && first.iterator_type == bite_iterator::iterator_type_t::scalar_i && last.iterator_type == bite_iterator::iterator_type_t::scalar_i)
			{
				bite();
				return first;
			}
			throw std::out_of_range("iterators for deletion out of range! or wrong type!");
		}

		return grain->erase(first, last, this);
	}


	void bite::clear()
	{
		bite(); //constructor call does not reset shared pointer!
		grain.reset(); //call to reset in constructor does not reset to NULL as well!
	}

	#define MILK_FLATTEN_DEFAULT_ESCAPE 0
	bite bite::flatten(char delimiter = '.', char escape = MILK_FLATTEN_DEFAULT_ESCAPE)
	{
		// nothing to flatten if we are no map
		if (!is_map())
			return *this;

		bite flat = std::map<std::string, bite>();

		// drop recursive lambda for calls to flatten of members instead: better interface but more copying
		// auto traverse = [&](milk::bite& map, std::string& nesting) { };

		for (auto&& it : *this)
		{
			if (!it.is_map() || (it.first.at(0) == escape && escape != MILK_FLATTEN_DEFAULT_ESCAPE))
				flat[it.first] = it.second; // copy non map container in flat map
			else
			// is map
			{
				bite nestedMap = it.flatten(delimiter, escape);
				for (auto&& nestedIt : nestedMap)
				{
					if (nestedIt.first.at(0) == escape && escape != MILK_FLATTEN_DEFAULT_ESCAPE)
					{
						flat[it.first][nestedIt.first] = nestedIt.second;
					}
					else
					{
						flat[it.first + delimiter + nestedIt.first] = nestedIt.second;
					}
				}
			}
		}

		return flat;
	}

	template<typename T>
	bite& bite::operator = (const T& other)
	{
		if (!grain)
			set<T>(other);
		else
			*grain = grain(other);

		return *this;
	}

	bite& bite::operator = (const char* other)
	{
		if (!grain)
			set(std::string(other));
		else
			*grain = grain(other);

		return *this;
	}

	bite& bite::operator = (std::vector<bite>& other)
	{
		if (!grain)
			set(other);
		else
			*grain = grain(other);

		return *this;
	}

	template<>
	bite& bite::operator = (const bite& other)
	{
		if (this == &other)
			return *this;
			//bite(other);

		if (!grain)
			grain = std::make_shared<grain>();

		*grain = *(other.grain);

		return *this;
	}

	template<>
	bite& bite::operator = (const bite_member_proxy& other)
	{
		if (grain == other.grain)
			return *this;

		if (!grain)
			grain = std::make_shared<grain>();
			//bite((milk::bite) other);

		grain = other.grain;

		return *this;
	}

	bite::~bite() {};

	std::shared_ptr<milk::grain> bite::get_grain_ptr() { return grain; };

	bite::bite(std::shared_ptr<milk::grain> grain_ptr)
	{
		grain = grain_ptr;
	}

	//should be refactored out - do not try to use!
	bite& bite::mutable_idx(std::string key)
	{
		if (!grain)
			set(std::map<std::string, bite>());
		// split calls as is_map() would dereference a nullpointer
		if(!is_map())
			set(std::map<std::string, bite>());

		return grain->idx(key);
	}


}
