#include "iterator_value.h"


namespace milk
{

	bite_iterator_value::bite_iterator_value() {};

	explicit bite_iterator_value::bite_iterator_value(std::string key, bite& val) :
		bite(val.get_grain_ptr()),
		std::pair<const std::string, const bite>(std::piecewise_construct, std::make_tuple(key), std::make_tuple(val.get_grain_ptr()))
	{};

	explicit bite_iterator_value::bite_iterator_value(bite& val) : bite_iterator_value("", val) {};

	template<typename O>
	bite& bite_iterator_value::operator = (const O& other)
	{
		return bite::operator=(other);
	};

	template<>
	bite& bite_iterator_value::operator = (const bite& other)
	{
		//return T::operator=(other);
		*(this->grain) = *(other.grain);
		return *this;
	};

	bite_iterator_value::~bite_iterator_value() {};


}