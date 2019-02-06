#include "bite_member_proxy.h"

namespace milk
{

	bite_member_proxy::bite_member_proxy(std::string key, bite& parent, const std::shared_ptr<milk::grain> data) : key(key), parent(parent), bite() // <-- grain(nullptr)
	{
		if (data)
			this->grain = data;
	};

	void bite_member_proxy::bin_extension(const unsigned char& val)
	{
		parent.mutable_idx(key).bin_extension(val);
	}

	bite& bite_member_proxy::mutable_idx(std::string key)
	{
		if (!this->grain)
			set(std::map<std::string, bite>());
		// split calls as is_map() would dereference a nullpointer
		if (!is_map())
			set(std::map<std::string, bite>());



		// we are a temporary container created by a request to return a map element, instantiate us in our holding parent element
		parent.mutable_idx(this->key) = *this;

		return this->grain->idx(key);
	}

	template<typename T>
	bite& bite_member_proxy::operator = (const T& other)
	{
		//set<T>(other);
		set(other);
		parent.mutable_idx(key) = *this;
		return (bite&) *this;
	}

	bite& bite_member_proxy::operator = (const char* ch)
	{
		//set<T>(other);
		set(std::string(ch));
		parent.mutable_idx(key) = *this;
		return (bite&) *this;
	}

	template<>
	bite& bite_member_proxy::operator = (const bite& other)
	{
		if (this->grain != other.grain)
		{
			if (this->grain)
				*(this->grain) = grain(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
			else
			{
				//set(other);
				this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
									 //
				parent.mutable_idx(key) = *this;
			}
		}

		return (bite)*this;
	}

	template<>
	bite& bite_member_proxy::operator = (const bite_member_proxy& other)
	{
		if (this->grain != other.grain)
		{
			if (grain)
				*(this->grain) = grain(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
			else
			{
				//set(other);
				this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
									 //
				parent.mutable_idx(key) = *this;
			}
		}

		return (bite)*this;
	}

	bite_member_proxy& bite_member_proxy::operator = (const bite_member_proxy& other)
	{
		if (this->grain != other.grain)
		{
			if(this->grain)
				*(this->grain) = grain(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
			else
			{
				//set(other);
				this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
				//
				parent.mutable_idx(key) = *this;
			}
		}

		return *this;
	}

}
