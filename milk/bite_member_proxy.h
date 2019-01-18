
namespace milk
{

	template<typename B>
	class bite_member_proxy_base : public B
	{
		friend typename B;

	private:
		milk::bite& parent;
		std::string key;

		bite_member_proxy_base(std::string key, milk::bite& parent, const std::shared_ptr<milk::grain_base<B>> data) : key(key), parent(parent), bite() // <-- grain(nullptr)
		{
			if (data)
				grain = data;
		};

	public:
		// overloading asignment operators and setter for binary extension to operate on original object

		void bin_extension(const unsigned char& val)
		{
			parent.mutable_idx(key).bin_extension(val);
		}

		milk::bite& mutable_idx(std::string key)
		{
			if (!grain)
				set(std::map<std::string, milk::bite>());
			// split calls as is_map() would dereference a nullpointer
			if (!is_map())
				set(std::map<std::string, milk::bite>());



			// we are a temporary container created by a request to return a map element, instantiate us in our holding parent element
			parent.mutable_idx(this->key) = *this;

			return grain->idx(key);
		}

		template<typename T>
		milk::bite& operator = (const T& other)
		{
			set<T>(other);
			parent.mutable_idx(key) = *this;
			return (milk::bite)*this;
		}

		template<>
		milk::bite& operator = (const milk::bite& other)
		{
			if (this->grain != other.grain)
			{
				if (grain)
					*grain = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
				else
				{
					//set(other);
					grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
										 //
					parent.mutable_idx(key) = *this;
				}
			}

			return (milk::bite)*this;
		}

		template<>
		milk::bite& operator = (const milk::bite_member_proxy& other)
		{
			if (this->grain != other.grain)
			{
				if (grain)
					*grain = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
				else
				{
					//set(other);
					grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
										 //
					parent.mutable_idx(key) = *this;
				}
			}

			return (milk::bite)*this;
		}

		milk::bite_member_proxy& operator = (const milk::bite_member_proxy& other)
		{
			if (this->grain != other.grain)
			{
				if(grain)
					*grain = milk::grain_base<B>(*(other.grain)); // should already set correct data if grain != 0 !? (grain is shared pointer)
				else
				{
					//set(other);
					grain = other.grain; // should already set correct data if grain != 0 !? (grain is shared pointer)
					//
					parent.mutable_idx(key) = *this;
				}
			}

			return *this;
		}

	};

}