#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iterator>

#include "types.h"
#include "bite.h"
#include "grain.h"
#include "iterator_value.h"

namespace milk
{
	// grain: friend, types
	// bite: pass pointer, hold pointer, friend
	// iterator_value: return by reference, return pointer, unique_ptr template
	class bite_iterator : public std::iterator<std::input_iterator_tag, bite_iterator_value>
	{
		// allow construction only from bite and grain classes
		friend class milk::grain;
		friend class milk::bite;

	private:
		milk::iterator_type_t iterator_type;

		// instantiated only on member access operator;
		// can not be instantiated on iterator_value instantiation as end() is not dereferenceable!
		std::unique_ptr<value_type> it_interface;

		typename grain::map_t::iterator map_iterator;
		typename grain::list_t::iterator list_iterator;

		bite* bite_ptr;
		bool end;

		// keep track of modifications to the iterator (not "referenced" data!)
		// avoid excessive iterator_value instantiations: ONLY ++ operator modifies iterator by now;
		bool modified;

		// map iterator
		bite_iterator(grain::map_t::iterator&&);
		// list iterator
		bite_iterator(grain::list_t::iterator&&);
		// scalar (non container bite); from bite
		bite_iterator(bite*, bool = false);


	public:

		bite_iterator & operator = (const bite_iterator&);

		bite_iterator(const bite_iterator&);

		bool operator == (const bite_iterator&) const;

		bool operator != (const bite_iterator&) const;

		bite_iterator& operator++();

		value_type& operator*();

		value_type* operator->();

	};
}
