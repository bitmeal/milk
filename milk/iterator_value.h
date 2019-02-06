#include <utility>
#include <string>

#include "bite.h"

namespace milk
{
	// bite: inherit, pass by reference, return by reference
	class bite_iterator_value : public bite, public std::pair<const std::string, const bite>
	{
		bite_iterator_value();

		explicit bite_iterator_value(std::string, bite&);
		explicit bite_iterator_value(bite&);

		template<typename O>
		bite& operator = (const O&);

		//template<>
		//bite& operator = (const bite&);

		~bite_iterator_value();
	};

}