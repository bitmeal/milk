
namespace milk
{
	template<typename B>
	struct bite_iterator_value : public B, public std::pair<const std::string, const B>
	{
		bite_iterator_value() {};
		explicit bite_iterator_value(std::string key, B& val) :
			B(val.get_grain_ptr()),
			std::pair<const std::string, const B>(std::piecewise_construct, std::make_tuple(key), std::make_tuple(val.get_grain_ptr()))
		{};
		explicit bite_iterator_value(B& val) : bite_iterator_value("", val) {};

		template<typename O>
		B& operator = (const O& other)
		{
			return B::operator=(other);
		};

		template<>
		B& operator = (const B& other)
		{
			//return T::operator=(other);
			*(this->grain) = *(other.grain);
			return *this;
		};
	};



	template<typename B, typename G>
	class bite_iterator_base : public std::iterator<std::input_iterator_tag, milk::bite_iterator_value<B>>
	{
		// allow construction only from bite and grain classes
		friend G;
		friend B;

		// ITERATOR typedef: iterator_category, value_type, difference_type, pointer, reference
		//typedef typename milk::bite_iterator_value<B> value_type;
		//typedef std::input_iterator_tag iterator_category;

	private:
		enum iterator_type_t {
			map_i,
			list_i,
			scalar_i
		};
		iterator_type_t iterator_type;

		// instantiated only on member access operator;
		// else, on use for dereferencing operator, this would not qualify const!
		// can not be instantiated on iterator instantiation as end() is not dereferenceable!
		std::unique_ptr<value_type> it_interface;

		typename G::map_t::iterator map_iterator;
		typename G::list_t::iterator list_iterator;

		B* bite_ptr;
		bool end;

		// keep track of modifications to the iterator (not "referenced" data!)
		// avoid excessive iterator_value instantiations: ONLY ++ operator modifies iterator by now;
		bool modified;

		// map iterator
		bite_iterator_base(typename G::map_t::iterator&& it) : iterator_type(map_i), map_iterator(it), modified(true) { };
		// list iterator
		bite_iterator_base(typename G::list_t::iterator&& it) : iterator_type(list_i), list_iterator(it), modified(true) { };
		// scalar (non container bite); from bite
		bite_iterator_base(B* data, bool end = false) : iterator_type(scalar_i), bite_ptr(data), end(end), modified(true) { };

		/*
		// scalar (non container grain); from grain
		bite_iterator(milk::grain data) : iterator_type(scalar_i) { };
		*/

	public:

		bite_iterator_base & operator = (const bite_iterator_base& other)
		{
			this->iterator_type = other.iterator_type;
			this->map_iterator = other.map_iterator;
			this->list_iterator = other.list_iterator;
			this->bite_ptr = other.bite_ptr;
			this->end = other.end;
			this->modified = true;

			return *this;
		};

		bite_iterator_base(const bite_iterator_base& other)
		{
			*this = other;
		};

		bool operator == (const milk::bite_iterator_base<B, G>& rhs) const
		{
			switch (iterator_type)
			{
			case map_i:
				return (map_iterator == rhs.map_iterator);
				break;
			case list_i:
				return (list_iterator == rhs.list_iterator);
				break;
			case scalar_i:
				return (bite_ptr == rhs.bite_ptr);
				break;
			}
		};

		bool operator != (const milk::bite_iterator_base<B, G>& rhs) const
		{
			switch (iterator_type)
			{
			case map_i:
				return (map_iterator != rhs.map_iterator);
				break;
			case list_i:
				return (list_iterator != rhs.list_iterator);
				break;
			case scalar_i:
				return (bite_ptr != rhs.bite_ptr);
				break;
			}
		};

		milk::bite_iterator& operator++()
		{
			switch (iterator_type)
			{
			case map_i:
				++map_iterator;
				break;
			case list_i:
				++list_iterator;
				break;
			case scalar_i:
				if (end) throw std::out_of_range("iterator out of range for scalar value!");
				end = true;
				++bite_ptr;
				break;
			}
			return *this;
		};

		value_type& operator*()
		{
			if (modified) {
				switch (iterator_type)
				{
				case map_i:
					it_interface = std::make_unique<value_type>(map_iterator->first, map_iterator->second);
					break;
				case list_i:
					it_interface = std::make_unique<value_type>(*list_iterator);
					break;
				case scalar_i:
					it_interface = std::make_unique<value_type>(*bite_ptr);
					break;
				default:
					it_interface = std::make_unique<value_type>();
				}
			}
			return *it_interface;
		};

		value_type* operator->()
		{
			if (modified) {
				switch (iterator_type)
				{
				case map_i:
					it_interface = std::make_unique<value_type>(map_iterator->first, map_iterator->second);
					break;
				case list_i:
					it_interface = std::make_unique<value_type>(*list_iterator);
					break;
				case scalar_i:
					it_interface = std::make_unique<value_type>(*bite_ptr);
					break;
				default:
					it_interface = std::make_unique<value_type>();
				}
			}
			return it_interface.get();
		};

	};
}
