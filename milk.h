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
	/*
	class bite;
	class grain;

	class bite_iterator;
	class bite_iterator_value;
	class bite_member_proxy;
	*/
}
// oder matters - do not modify!
#include "milk/binary_proxy.h"

#include "milk/bite.h"

//#include "milk/grain.h"
//
//#include "milk/bite_member_proxy.h"
//#include "milk/bite_iterator.h"

