
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
		// overloading asignment operators to operate on original object

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
				bite(other);
				parent.mutable_idx(key) = *this;
			}

			return (milk::bite)*this;
		}

		template<>
		milk::bite& operator = (const milk::bite_member_proxy& other)
		{
			if (this->grain != other.grain)
			{
				//set(other);
				grain = other.grain;
				//
				parent.mutable_idx(key) = *this;
			}

			return (milk::bite)*this;
		}

	};

}