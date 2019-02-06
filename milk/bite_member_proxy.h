#pragma once

#include<string>
#include<memory>

namespace milk
{

	class bite_member_proxy : public milk::bite
	{
		friend class milk::bite;

	private:
		milk::bite& parent;
		std::string key;

		bite_member_proxy(std::string key, milk::bite& parent, const std::shared_ptr<milk::grain> data);

	public:
		// overloading asignment operators and setter for binary extension to operate on original object

		void bin_extension(const unsigned char&);

		milk::bite& mutable_idx(std::string);

		template<typename T>
		milk::bite& operator = (const T&);

		milk::bite& operator = (const char*);

		//template<>
		//bite& operator = (const bite&);
		//
		//template<>
		//bite& operator = (const bite_member_proxy&);

		bite_member_proxy& operator = (const bite_member_proxy&);

	};

}
