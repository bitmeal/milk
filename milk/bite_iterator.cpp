#include "bite_iterator.h"

namespace milk
{
	// map iterator
	bite_iterator::bite_iterator(grain::map_t::iterator&& it) : iterator_type(map_i), map_iterator(it), modified(true) { };
	// list iterator
	bite_iterator::bite_iterator(grain::list_t::iterator&& it) : iterator_type(list_i), list_iterator(it), modified(true) { };
	// scalar (non container bite); from bite
	bite_iterator::bite_iterator(bite* data, bool end = false) : iterator_type(scalar_i), bite_ptr(data), end(end), modified(true) { };


	bite_iterator& bite_iterator::operator = (const bite_iterator& other)
	{
		this->iterator_type = other.iterator_type;
		this->map_iterator  = other.map_iterator;
		this->list_iterator = other.list_iterator;
		this->bite_ptr	    = other.bite_ptr;
		this->end		    = other.end;
		this->modified	    = true;

		return *this;
	};

	bite_iterator::bite_iterator(const bite_iterator& other)
	{
		*this = other;
	};

	bool bite_iterator::operator == (const bite_iterator& rhs) const
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

	bool bite_iterator::operator != (const bite_iterator& rhs) const
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

	bite_iterator& bite_iterator::operator++()
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

	bite_iterator::value_type& bite_iterator::operator*()
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

	bite_iterator::value_type* bite_iterator::operator->()
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

}
