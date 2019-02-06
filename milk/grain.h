#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "types.h"

namespace milk
{
	// bite: template argument, pointer, ref
	// bite_iterator: value
	class grain
	{
		public:
			typedef std::vector<unsigned char>			str_bin_t;
			typedef std::map<std::string, milk::bite>	map_t;
			typedef std::vector<milk::bite>				list_t;

		private:
			union t_scalar_data {
				int64_t			d_int;
				double			d_fp;
				unsigned char	d_byte;
				bool			d_bool;
			};
			t_scalar_data scalar_data;
			milk::t_type type;

			unsigned char bin_ext;

			int l_str_bin;
			std::unique_ptr<str_bin_t>	d_str_bin;
			std::unique_ptr<map_t>		d_map;
			std::unique_ptr<list_t>		d_list;


		public:
			// DEFAULT
			grain();

			// COPY CONSTRUCTOR
			grain(const grain&);

			grain& operator=(const grain&);

			/*
			CONSTRUCTORS - SETTERS
			 - integral types + char/uchar
			 - floating point types
			 - std::string
			 - string from char*
			 - binary from iterable char/uchar container
			 - binary from object with size_t size() and char* data()
			 - iterable container
			 - std::map<std::string, <T>>
			 - bool
			*/

			// integral types (bool has specialization - why?)
			template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
			grain(const T&);

			// floating point types
			template <typename T, std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
			grain(const T&);

			// map types
			template<typename T, std::enable_if_t<
				sizeof(typename T::key_type) != 0
			>* = nullptr >
			grain(const T&);

			// container types with iterators
			template<typename T, std::enable_if_t<
				!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
				!std::is_same<decltype(std::declval<T>().end()), void>::value &&
				!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value &&
				!std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value
			>* = nullptr>
			grain(const T&);

			// container types with iterators and char data; interpreted as binary!
			template<typename T, std::enable_if_t<
				!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
				!std::is_same<decltype(std::declval<T>().end()), void>::value &&
				(std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, char*>::value ||
					std::is_same<std::remove_cv_t<decltype(std::declval<T>().data())>, unsigned char*>::value)
			>* = nullptr>
			grain(const T&);

			// has size_t size() and char* data()
			template<typename T, std::enable_if_t<
				!std::is_same<std::string, T>::value &&
				std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
				(std::is_same<const char*, decltype(std::declval<T>().data())>::value ||
				std::is_same<const unsigned char*, decltype(std::declval<T>().data())>::value)
			>* = nullptr>
			grain(const T&);

			// handling: milk::bite my_bite = "string/chararr";
			grain(const char* &);

			// handling: char* ch = "chararr"; milk::bite my_bite = ch;
			template <typename T, std::enable_if_t<std::is_same<char*, std::remove_const_t<T>>::value>* = nullptr>
			grain(const T&);

			//template<>
			//grain(const std::string&);
			//
			//template<>
			//grain(const bool&);

			void bin_extension(const unsigned char&);

			/*
			GETTERS
			attempting auto conversion here. throws if not possible!
			 - arithmetic types
			 - uchar
			 - uchar vector (binary), conversion from all types
			 - std::string
			 - bool; conversion from all types with: false if value or size of held type = 0, true otherwise

			*/

			t_type get_type() const;

			// CATCH INCOMPATIBLE CONVERSIONS AT COMPILE TIME!
			template <typename T>
			typename std::enable_if<!std::is_arithmetic<T>::value, T>::type
			get() const;

			template <typename T>
			typename std::enable_if<std::is_arithmetic<T>::value, T>::type
			get() const;

			//template<>
			//unsigned char get<unsigned char>() const;
			//
			//template<>
			//binary_proxy get<binary_proxy>() const;
			//
			//template<>
			//std::vector<unsigned char> get<std::vector<unsigned char>>() const;
			//
			//template<>
			//std::string get<std::string>() const;
			//
			//template<>
			//bool get<bool>() const;

			unsigned char bin_extension() const;

			bool is_map() const;

			bool is_list() const;

			bool is_scalar() const;

			bool is_fp() const;

			bool is_float() const;

			bool is_int() const;

			bool is_bool() const;

			bool is_byte() const;

			bool is_str() const;

			bool is_bin() const;


			std::size_t size() const;

			const unsigned char* data() const;

			bite_iterator begin(bite*);

			bite_iterator end(bite*);

			bite_iterator find(const std::string&, bite*);

			// throws out of range exception or runtime error when called on non container grain
			bite& idx(const std::size_t);

			bite& back();

			bite& front();

			bool idx_probe(const std::string&);

			bite& idx(const std::string&);

			template<typename T>
			void push_back(const T&);

			void pop_back();

			std::size_t erase(const std::string&);

			bite_iterator erase(bite_iterator, bite_iterator, bite*);

			~grain() {};
	};

	typedef grain::t_type type;
}
