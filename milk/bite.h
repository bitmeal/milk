#pragma once

#include <memory>
#include <string>
#include <vector>

#include "types.h"

// bite_iterator: pass by value, return by value, friend
// bite_member_proxy: return by value, typeinfo, friend
// grain: shared_ptr template, pass by reference

namespace milk
{
	//class bite_iterator;
	//class bite_iterator_value;
	//class bite_member_proxy;
	//class grain;
	
	// todo: add handling for grain == null (un initialized, empty object)
	class bite
	{
		friend class milk::bite_member_proxy;
		friend class milk::bite_iterator_value;

		private:
			std::shared_ptr<milk::grain> grain;
		public:
			bite();

			bite(const milk::grain&);
			bite(const bite&);

			// constructor using value initialization; templated to avoid accidental initialization from bite_member_proxy types!
			template<typename T, std::enable_if_t<
				!std::is_same<milk::bite, T>::value &&
				!std::is_same<milk::bite_member_proxy, T>::value
			>* = nullptr>
				bite(T&);

			template<typename T>
			void set(const T);

			void bin_extension(const unsigned char&);
			
			template<typename T>
			T get() const;

			unsigned char bin_extension() const;

			milk::type type()	const;


			bool is_map() 		const;
			bool is_list() 		const;
			bool is_scalar()	const;

			bool is_fp() 		const;
			bool is_float() 	const;
			bool is_int() 		const;
			bool is_bool() 		const;
			bool is_byte() 		const;
			bool is_str() 		const;
			bool is_string() 	const;
			bool is_bin() 		const;
			bool is_binary() 	const;


			std::size_t size() const;
			bool empty() const;

			const unsigned char* data() const;

			// iterators
			bite_iterator begin();
			bite_iterator end();

			// find(key); returns end iterator if not found and not map!
			bite_iterator find(const std::string&);

			// at() and operator[] list index access
			milk::bite at(const std::size_t) const;

			milk::bite& at(const std::size_t);

			milk::bite operator [](const std::size_t) const;

			milk::bite& operator [](const std::size_t);

			milk::bite& back();
			milk::bite& front();

			// at() and operator[] map key access
			milk::bite_member_proxy at(const std::string&);

			milk::bite at(const std::string&) const;

			// doubles functionality with protected mutable_idx(key) for iterators
			// implemented for efficient construction from serialization formats - needs review
			milk::bite& bite_ref_at(const std::string&);

			milk::bite_member_proxy operator [](const std::string&);

			milk::bite operator [](const std::string&) const;


			template<typename T>
			void push_back(T);

			void pop_back();

			std::size_t erase(const std::string&);

			bite_iterator erase(bite_iterator, bite_iterator);

			// resets the whole milk::bite container
			void clear();

			// flattens nested map structurec to a top level map
			// concattenates keys by <delimiter>
			// keys can be protected from flattening by a leading escape character
			#define MILK_FLATTEN_DEFAULT_ESCAPE 0
			milk::bite flatten(char = '.', char = MILK_FLATTEN_DEFAULT_ESCAPE);

			template<typename T>
			milk::bite& operator = (const T&);

			milk::bite& operator = (const char*);

			milk::bite& operator = (std::vector<milk::bite>&);

			//template<>
			//milk::bite& operator = (const milk::bite&);
			//
			//template<>
			//milk::bite& operator = (const milk::bite_member_proxy&);

			~bite() {};

			protected:
				std::shared_ptr<milk::grain> get_grain_ptr();
			public:
				bite(std::shared_ptr<milk::grain>);

				//should be refactored out - do not try to use!
				virtual milk::bite& mutable_idx(std::string);
		};

}
