#include <typeinfo>
#include <type_traits>

#include <memory>

#include <iterator>

#include <array>
#include <vector>
#include <string>
#include <map>
#include <utility>

#include <exception>
#include <stdexcept>


namespace milk
{
	// templated classes for circular dependency resolution
	// oder matters - do not modify!

	template<typename B, typename G>
	class bite_iterator_base;

	template<typename B>
	class grain_base;

	template<typename B>
	class bite_member_proxy_base;

	class bite;

	typedef bite_member_proxy_base<milk::bite> bite_member_proxy;
	typedef grain_base<milk::bite> grain;
	typedef bite_iterator_base<milk::bite, milk::grain> bite_iterator;

}

// oder matters - do not modify!
#include "milk/binary_proxy.h"
#include "milk/grain.h"
namespace milk { typedef grain::t_type type; }
#include "milk/bite_iterator.h"
#include "milk/bite_member_proxy.h"
#include "milk/bite.h"

