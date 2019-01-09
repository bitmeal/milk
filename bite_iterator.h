#include <utility>
#include <string>
#include <stdexcept>
#include <iterator>

namespace milk
{
	struct bite_iterator_value : public milk::bite, public std::pair<std::string, milk::bite>
	{
		bite_iterator_value() {};
		explicit bite_iterator_value(const std::string key, const milk::bite& val) : milk::bite(val), std::pair<std::string, milk::bite>(key, val) {};
		explicit bite_iterator_value(const milk::bite& val) : bite_iterator_value("", val){};
	};

	class bite_iterator : public std::iterator<std::input_iterator_tag, milk::bite_iterator_value>
	{
		//friend class milk::grain;
		//friend class milk::bite;
	
	private:
		enum iterator_type_t {
			map_i,
			list_i,
			scalar_i
		};
		iterator_type_t iterator_type;

		milk::grain::map_t::iterator map_iterator;
		milk::grain::list_t::iterator list_iterator;

		milk::bite* bite_ptr;
		bool end;


		// map iterator
		bite_iterator(milk::grain::map_t::iterator& it) : iterator_type(map_i), map_iterator(it)	{ };
		// list iterator
		bite_iterator(milk::grain::list_t::iterator& it) : iterator_type(list_i), list_iterator(it) { };
		// scalar (non container bite); from bite
		bite_iterator(milk::bite* data, bool end = false) : iterator_type(scalar_i), bite_ptr(data), end(end) {	};

		/*
		// scalar (non container grain); from grain
		bite_iterator(milk::grain data) : iterator_type(scalar_i) { };
		*/

	public:
		bool operator == (const milk::bite_iterator& rhs) const
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

		bool operator != (const milk::bite_iterator& rhs) const
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

		/*
		value_type* operator->() const
		{

		};
		*/

		bool operator

	};
}