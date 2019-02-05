
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
		explicit bite_iterator_value(B& val) : bite_iterator_value("", val){};

		template<typename O, std::enable_if_t<!std::is_same<B, O>::value>>
		B& operator = (const O& other)
		{
			return B::operator=(other);
		};

		template<typename T>
		typename std::enable_if_t<std::is_same<B, T>::value>
		& operator = (const B& other)
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
		typedef typename std::iterator<std::input_iterator_tag, milk::bite_iterator_value<B>>::value_type value_type;
		value_type it_interface;

		typename G::map_t::iterator map_iterator;
		typename G::list_t::iterator list_iterator;

		B* bite_ptr;
		bool end;

		// map iterator
		bite_iterator_base(typename G::map_t::iterator&& it) : iterator_type(map_i), map_iterator(it) { };
		// list iterator
		bite_iterator_base(typename G::list_t::iterator&& it) : iterator_type(list_i), list_iterator(it) { };
		// scalar (non container bite); from bite
		bite_iterator_base(B* data, bool end = false) : iterator_type(scalar_i), bite_ptr(data), end(end) { };

		/*
		// scalar (non container grain); from grain
		bite_iterator(milk::grain data) : iterator_type(scalar_i) { };
		*/

	public:
		bool operator == (const milk::bite_iterator_base<B,G>& rhs) const
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

		bool operator != (const milk::bite_iterator_base<B,G>& rhs) const
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

		value_type operator*() const
		{
			switch (iterator_type)
			{
			case map_i:
				return value_type(map_iterator->first, map_iterator->second);
				break;
			case list_i:
				return value_type(*list_iterator);
				break;
			case scalar_i:
				return value_type(*bite_ptr);
				break;
			}
			return value_type();
		};

		value_type* operator->()
		{
			switch (iterator_type)
			{
			case map_i:
				this->it_interface = value_type(map_iterator->first, map_iterator->second);
				break;
			case list_i:
				this->it_interface = value_type(*list_iterator);
				break;
			case scalar_i:
				this->it_interface = value_type(*bite_ptr);
				break;
			}
			//it_interface = value_type();

			return &it_interface;
		};

	};
}
