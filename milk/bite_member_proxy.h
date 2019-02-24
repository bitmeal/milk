#pragma once

namespace milk
{

	template<typename B>
	class bite_member_proxy_base : public B
	{
		friend B;

	private:
		milk::bite& parent;
		std::string key;

		bite_member_proxy_base(std::string key, milk::bite& parent, const std::shared_ptr<milk::grain_base<B>> data) : key(key), parent(parent), bite() // <-- grain(nullptr)
		{
			if (data)
				this->grain = data;
		};

	public:
		// overloading asignment operators and setter for binary extension to operate on original object

		void bin_extension(const unsigned char& val)
		{
			parent.mutable_idx(key).bin_extension(val);
		}

		B& mutable_idx(std::string key)
		{
			if (!this->grain)
				this->set(std::map<std::string, B>());
			// split calls as is_map() would dereference a nullpointer
			if (!this->is_map())
				this->set(std::map<std::string, B>());



			// we are a temporary container created by a request to return a map element, instantiate us in our holding parent element
			parent.mutable_idx(this->key) = *this;

			return this->grain->idx(key);
		}

		template<typename T>
		const B& operator = (const T& other)
		{
			//set<T>(other);
			this->set(other);
			parent.mutable_idx(key) = *this;
			return (B&) *this;
		}

		const B& operator = (const char* ch)
		{
			//set<T>(other);
			this->set(std::string(ch));
			parent.mutable_idx(key) = *this;
			return (B&) *this;
		}

		//template<>
		const B& operator = (const B& other)
		{
			if (this->grain != other.grain)
			{
				if (this->grain)
					*(this->grain) = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
				else
				{
					//set(other);
					this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
										 //
					parent.mutable_idx(key) = *this;
				}
			}

			return (B)*this;
		}

		//template<>
		const B& operator = (const milk::bite_member_proxy_base<B>& other)
		{
			if (this->grain != other.grain)
			{
				if (this->grain)
					*(this->grain) = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
				else
				{
					//set(other);
					this->grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
										 //
					parent.mutable_idx(key) = *this;
				}
			}

			return (B)*this;
		}

// SHOULD NOT BE NECCESSARY
/*
		bite_member_proxy_base<B>& operator = (const bite_member_proxy_base<B>& other)
		{
			if (this->grain != other.grain)
			{
				if(this->grain)
					*(this->grain) = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
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
*/
	};

}
