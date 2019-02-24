#pragma once

#include <typeinfo>
#include <type_traits>

#include <memory>

#include <iterator>

#include <array>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <tuple>

#include <exception>
#include <stdexcept>


namespace milk
{
	// templated classes for circular dependency resolution
	// oder matters - do not modify!

	template<typename B, typename G>
	class bite_iterator_base;
	//template<typename B, typename G>
	//enum bite_iterator_base<B,G>::iterator_type_t;

	template<typename B>
	class grain_base;
	//template<typename B>
	//enum grain_base<B>::t_type;

	template<typename B>
	class bite_member_proxy_base;

	class bite;

	typedef bite_member_proxy_base<milk::bite> bite_member_proxy;
	typedef grain_base<milk::bite> grain;
	typedef bite_iterator_base<milk::bite, milk::grain> bite_iterator;

}

// oder matters - do not modify!
#include "milk/types.h"
#include "milk/binary_proxy.h"
#include "milk/bite_member_proxy.h"
#include "milk/grain.h"
#include "milk/bite_iterator.h"
#include "milk/bite.h"

namespace milk
{

	// ITERATOR value operator = overload
	template<>
	template<>
	inline
	milk::bite& bite_iterator_value<milk::bite>::operator = <milk::bite>(const milk::bite& other)
	{
		//return T::operator=(other);
		*(this->grain) = *(other.grain);
		return *this;
	};

}
