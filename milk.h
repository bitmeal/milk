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
	//typedef bite_iterator_base<milk::bite, milk::grain>::iterator_type_t bite_iterator::iterator_type_t;

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

	// MEMBER_PROXY
	/*
	template<>
	template<>
	milk::bite& bite_member_proxy_base<milk::bite>::operator = <milk::bite>(const milk::bite& other)
	{
		if (this->grain != other.grain)
		{
			if (this->grain)
				*(this->grain) = milk::grain_base<milk::bite>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
			else
			{
				//set(other);
				this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
									 //
				parent.mutable_idx(key) = *this;
			}
		}

		return (milk::bite)*this;
	};

	template<>
	template<>
	milk::bite& bite_member_proxy_base<milk::bite>::operator = <milk::bite>(const milk::bite_member_proxy& other)
	{
		if (this->grain != other.grain)
		{
			if (grain)
				*(this->grain) = milk::grain_base<milk::bite>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
			else
			{
				//set(other);
				this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
									 //
				parent.mutable_idx(key) = *this;
			}
		}

		return (milk::bite)*this;
	};
	*/

	/*
	// GRAIN
	template<>
	template<>
	grain_base<milk::bite>::grain_base<std::string>(const std::string& val) : grain_base()
	{
		type = n_str;
		d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
	};

	template<>
	template<>
	grain_base<milk::bite>::grain_base<bool>(const bool& val) : grain_base()
	{
		type = s_bool;
		scalar_data.d_bool = val;
	};
	*/

	// ITERATOR value
	template<>
	template<>
	milk::bite& bite_iterator_value<milk::bite>::operator = <milk::bite>(const milk::bite& other)
	{
		//return T::operator=(other);
		*(this->grain) = *(other.grain);
		return *this;
	};


}
