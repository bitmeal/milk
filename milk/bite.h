#pragma once

namespace milk
{
	// todo: add handling for grain == null (un initialized, empty object)
	class bite
	{
		friend class bite_member_proxy_base<milk::bite>;
		friend bite_iterator_value<milk::bite>;
		// TODO: befriend std::pair and/or std::pair constructor for in place instantiation of bite from grain shared pointer-> make bite construtor from shared pointer protected; used in iterator to avoid copying
		//friend class std::pair<std::string, milk::bite>;
		//friend auto std::pair<const std::string, const milk::bite>::pair<std::string, std::shared_ptr<milk::grain>>(std::piecewise_construct_t, std::tuple<std::string>, std::tuple < std::shared_ptr<milk::grain>>);

		private:
			std::shared_ptr<milk::grain> grain;
		public:
			bite() : grain(nullptr) { };

			// bite is an interface for grain and holds no data other than a grain; allows construction purely from grain
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
			bite(const T& val)
			{
				set(val);
			};

			template<typename T>
			void set(const T& val) {
				if(!grain)
					grain = std::make_shared<milk::grain>(val);
				else
					*grain = milk::grain(val);
			};

			void bin_extension(const unsigned char& val)
			{
				grain->bin_extension(val);
			}

			template<typename T>
			T get() const {
				return grain ? grain->get<T>() : T();
			};

			unsigned char bin_extension() const
			{
				return grain->bin_extension();
			}

			milk::t_type type() const { return grain->get_type(); }


			bool is_map() 		const { return grain ? 	grain->is_map()		: false;}
			bool is_list() 		const { return grain ? 	grain->is_list()	: false;}
			bool is_scalar()	const { return grain ? 	grain->is_scalar()	: false;}

			bool is_fp() 		const { return grain ? 	grain->is_fp()		: false;}
			bool is_float() 	const { return 			is_fp(); 					}
			bool is_int() 		const { return grain ? 	grain->is_int()		: false;}
			bool is_bool() 		const { return grain ? 	grain->is_bool()	: false;}
			bool is_byte() 		const { return grain ? 	grain->is_byte()	: false;}
			bool is_str() 		const { return grain ? 	grain->is_str()		: false;}
			bool is_string() 	const { return 			is_str(); 					}
			bool is_bin() 		const { return grain ? 	grain->is_bin()		: false;}
			bool is_binary() 	const { return 			is_bin(); 					}


			std::size_t size() const { return grain ? grain->size() : 0; };
			bool empty() { return (size() == 0); }

			// for binary and string data only - else will return nullptr!
			const unsigned char* data() const { return grain ? grain->data() : nullptr; };

			// iterators
			bite_iterator begin()
			{
				return grain ? grain->begin(this) : bite_iterator(this);
			}

			bite_iterator end()
			{
				return grain ? grain->end(this) : bite_iterator(this);
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

			milk::bite& back() { return grain->is_scalar() ? *this : grain->back(); }
			milk::bite& front() { return grain->is_scalar() ? *this : grain->front(); }

			// at() and operator[] map key access
			milk::bite_member_proxy at(const std::string& key)
			{
				return milk::bite_member_proxy(key, *this, grain ? ( grain->idx_probe(key) ? grain->idx(key).grain : std::shared_ptr<milk::grain_base<milk::bite>>(nullptr) ) : std::shared_ptr<milk::grain_base<milk::bite>>(nullptr) );
			};

			milk::bite at(const std::string& key) const
			{
				return grain ? ( grain->idx_probe(key) ? grain->idx(key) : milk::bite() ) : milk::bite();
			};

			// doubles functionality with protected mutable_idx(key) for iterators
			// implemented for efficient construction from serialization formats - needs review
			milk::bite& bite_ref_at(const std::string& key)
			{
				//makes this instance a map, not matter what! and inserts the requested key!
				if(!grain)
					grain = std::make_shared<milk::grain>();

				return grain->idx(key);
			};

			milk::bite_member_proxy operator [](const std::string& key) { return at(key); };

			milk::bite operator [](const std::string& key) const { return at(key); };


			template<typename T>
			void push_back(T val)
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
					bite_vec.push_back(*this);
					bite_vec.push_back(milk::bite(val));
					set(bite_vec);
					return;
				}

				// leave handling to grain
				grain->push_back<T>(val);
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
					if (first == begin() && last == end() && first.iterator_type == milk::iterator_type_t::scalar_i && last.iterator_type == milk::iterator_type_t::scalar_i)
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
				bite(); //constructor call does not reset shared pointer!
				grain.reset(); //call to reset in constructor does not reset to NULL as well!
			}

			// flattens nested map structurec to a top level map
			// concattenates keys by <delimiter>
			// keys can be protected from flattening by a leading escape character
			#define MILK_FLATTEN_DEFAULT_ESCAPE 0
			milk::bite flatten(char delimiter = '.', char escape = MILK_FLATTEN_DEFAULT_ESCAPE)
			{
				// nothing to flatten if we are no map
				if (!is_map())
					return *this;

				// SHOULD WORK WITHOUT PRIOR MAP ASSIGNMENT
				milk::bite flat;// = std::map<std::string, milk::bite>();

				// drop recursive lambda for calls to flatten of members instead: better interface but more copying
				// auto traverse = [&](milk::bite& map, std::string& nesting) { };

				for (auto&& it : *this)
				{
					if (!it.is_map() || (it.first.at(0) == escape && escape != MILK_FLATTEN_DEFAULT_ESCAPE))
						flat[it.first] = it.second; // copy non map container in flat map
					else
					// is map
					{
						milk::bite nestedMap = it.flatten(delimiter, escape);
						for (auto&& nestedIt : nestedMap)
						{
							if (nestedIt.first.at(0) == escape && escape != MILK_FLATTEN_DEFAULT_ESCAPE)
							{
								flat[it.first][nestedIt.first] = nestedIt.second;
							}
							else
							{
								flat[it.first + delimiter + nestedIt.first] = nestedIt.second;
							}
						}
					}
				}

				return flat;
			}

			template<typename T>
			milk::bite& operator = (const T& other)
			{
				if (!grain)
					set<T>(other);
				else
					*grain = milk::grain(other);

				return *this;
			}

			milk::bite& operator = (const char* other)
			{
				if (!grain)
					set(std::string(other));
				else
					*grain = milk::grain(other);

				return *this;
			}

			milk::bite& operator = (std::vector<milk::bite>& other)
			{
				if (!grain)
					set(other);
				else
					*grain = milk::grain(other);

				return *this;
			}

			~bite() {};

			protected:
				std::shared_ptr<milk::grain> get_grain_ptr() { return grain; };
			public:
				bite(std::shared_ptr<milk::grain> grain_ptr)
				{
					grain = grain_ptr;
				}

				//should be refactored out - do not try to use!
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




		// "outlining" template specializations
		template<>
		inline milk::bite& bite::operator = <milk::bite>(const milk::bite& other)
		{
			if (this == &other)
				return *this;
			//bite(other);

			if (!grain)
				grain = std::make_shared<milk::grain>();

			*grain = *(other.grain);

			return *this;
		}

		template<>
		inline milk::bite& bite::operator = <milk::bite_member_proxy>(const milk::bite_member_proxy& other)
		{
			if (grain == other.grain)
				return *this;

			if (!grain)
				grain = std::make_shared<milk::grain>();
			//bite((milk::bite) other);

			grain = other.grain;

			return *this;
		}


}
