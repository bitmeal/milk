
namespace milk
{
	// todo: add handling for grain == null (un initialized, empty object)
	class bite
	{
		friend class bite_member_proxy_base<milk::bite>;

		private:
			std::shared_ptr<milk::grain> grain;
		public:
			bite() : grain(nullptr) {};

			// bite is an interface for grain and holds no data other than a grain; allows construction from grain
			bite(const milk::grain& source)
			{
				//grain.release();
				grain = std::make_shared<milk::grain>(source);
			}

			bite(const bite& source)
			{
				//grain.release();
				if(source.grain)
					grain = std::make_shared<milk::grain>(*(source.grain.get()));

			};

			// constructor using value initialization; templated to avoid accidental initialization from bite_member_proxy types!
			template<typename T, std::enable_if_t<
				!std::is_same<milk::bite, T>::value &&
				!std::is_same<milk::bite_member_proxy_base<milk::bite>, T>::value
			>* = nullptr>
				bite(T val)
			{
				set(val);
			};		

			template<typename T>
			void set(const T val) {
				//grain.release();
				grain = std::make_shared<milk::grain>(val);
			};

			
			template<typename T>
			T get() {
				return grain ? grain->get<T>() : T();
			};

			bool is_map() { return grain->is_map(); };
			bool is_list() { return grain->is_list(); };
			bool is_scalar() { return grain->is_scalar(); };

			std::size_t size() { return grain ? grain->size() : 0; };
			bool empty() { return (size() == 0); }

			// iterators
			bite_iterator begin()
			{
				return grain->begin(this);
			}

			bite_iterator end()
			{
				return grain->end(this);
			}

			// find(key); returns end iterator if not found and not map!
			bite_iterator find(const std::string& key)
			{
				return grain->find(key, this);
			}

			// at() and operator[] list index access
			milk::bite at(const std::size_t idx) const
			{
				return grain->is_scalar() ? *this : grain->idx(idx);
			};

			milk::bite& at(const std::size_t idx)
			{
				return grain->is_scalar() ? *this : grain->idx(idx);
			};

			milk::bite operator [](const std::size_t idx) const {	return at(idx);	};

			milk::bite& operator [](const std::size_t idx) { return at(idx); };

			// operator[] map key access
			milk::bite_member_proxy at(const std::string& key)
			{
				return milk::bite_member_proxy(key, *this, grain ? grain->idx_probe(key).grain : std::shared_ptr<milk::grain_base<milk::bite>>(nullptr));
			};

			milk::bite at(const std::string& key) const
			{
				return grain ? grain->idx_probe(key) : milk::bite();
			};

			milk::bite_member_proxy operator [](const std::string& key) { return at(key); };

			milk::bite operator [] (const std::string& key) const { return at(key); };


			template<typename T>
			void push_back(T& val)
			{
				std::vector<milk::bite> bite_vec;
				
				// instantiate new
				if (!grain)
				{
					bite_vec.push_back(milk::bite(val));
					set(bite_vec);
					return;
				}

				// implicit conversion from scalar
				if (is_scalar())
				{
					bite_vec.push_back(this*);
					bite_vec.push_back(milk::bite(val));
					return;
				}
					
				// leave handling to grain
				grain->push_back<T>(T);
			};

			void pop_back()
			{
				grain->pop_back();
			}

			std::size_t erase(const std::string& key)
			{
				return grain->erase(key);
			}

			bite_iterator erase(bite_iterator first, bite_iterator last)
			{
				// range = 0
				if (first == last)
				{
					if (last == end())
						return last;
					else
						return ++last;
				}

				if (is_scalar())
				{
					if (first == begin() && last == end() && first.iterator_type == bite_iterator::iterator_type_t::scalar_i && last.iterator_type == bite_iterator::iterator_type_t::scalar_i)
					{
						bite();
						return first;
					}
					throw std::out_of_range("iterators for deletion out of range! or wrong type!");
				}

				return grain->erase(first, last, this);
			}


			// resets the whole milk::bite container
			void clear()
			{
				bite();
			}


			/*
			milk::bite& operator ()(std::string key) 
			{
				if (!grain)
					grain = std::make_shared<milk::grain>();
				return grain->idx(key);
			};
			*/
			//todo: overload (int) for list access and insertion!

			//todo: add push_back for list types

			template<typename T>
			milk::bite& operator = (const T& other)
			{
				set<T>(other);
				return *this;
			}

			template<>
			milk::bite& operator = (const milk::bite& other)
			{
				if (this != &other)
					bite(other);

				return *this;
			}

			template<>
			milk::bite& operator = (const milk::bite_member_proxy& other)
			{
				if (grain != other.grain)
					//bite((milk::bite) other);
					grain = other.grain;
				
				return *this;
			}

			~bite() {};

			protected:
				virtual milk::bite& mutable_idx(std::string key)
				{
					if (!grain)
						set(std::map<std::string, milk::bite>());
					// split calls as is_map() would dereference a nullpointer
					if(!is_map())
						set(std::map<std::string, milk::bite>());

					return grain->idx(key);
				}
		};

}

