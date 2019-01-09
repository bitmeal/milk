#include <type_traits>

#include <memory>

#include <string>
#include <array>
#include <vector>
#include <map>

#include <exception>
#include <typeinfo>

//#include "bite.h"
#include <cassert>

namespace milk
{

	class grain
	{
		public:
			typedef std::vector<unsigned char>			str_bin_t;
			typedef std::map<std::string, milk::bite>	map_t;
			typedef std::vector<milk::bite>				list_t;


		private:
			union t_scalar_data {
				int64_t			d_int;
				double_t		d_fp;
				unsigned char	d_byte;
				bool			d_bool;
			};
			enum t_type {
				s_int,
				s_fp,
				s_byte,
				s_bool,
				n_str,
				n_bin,
				t_map,
				t_list,
			};

			t_scalar_data scalar_data;
			grain::t_type type;
			
			char bin_ext;

			int l_str_bin;
			std::unique_ptr<str_bin_t>	d_str_bin;
			std::unique_ptr<map_t>		d_map;
			std::unique_ptr<list_t>		d_list;
			

		public:
			// DEFAULT
			grain() {};

			// COPY CONSTRUCTOR
			grain(const grain& source)
			{
				type = source.type;
				scalar_data = source.scalar_data;
				bin_ext = source.bin_ext;

				switch (type)
				{
				case t_map:
					d_map = std::make_unique<map_t>(*(source.d_map.get()));
					break;
				case t_list:
					d_list = std::make_unique<list_t>(*(source.d_list.get()));
					break;
				case n_str:
				case n_bin:
					d_str_bin = std::make_unique<str_bin_t>(*(source.d_str_bin.get()));
					break;
				}
			};

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
			grain(const T& val)
			{
				if (typeid(T) == typeid(char) || typeid(T) == typeid(unsigned char))
				{
					type = s_byte;
					scalar_data.d_byte = val;
					return;
				}
				
				type = s_int;
				scalar_data.d_int = val;
				return;
			};

			// floating point types
			template <typename T, std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
			grain(const T& val)
			{
					type = s_fp;
					scalar_data.d_fp = val;
					return;
			};
			
			// map types
			template<typename T, std::enable_if_t<
				sizeof(typename T::key_type) != 0
			>* = nullptr >
				grain(const T& val)
			{
				type = t_map;
				d_map = std::make_unique<map_t>(val.begin(), val.end());
			};

			// container types with iterators
			template<typename T, std::enable_if_t<
				!std::is_same<decltype(std::declval<T>().begin()), void>::value &&
				!std::is_same<decltype(std::declval<T>().end()), void>::value
				>* = nullptr,
				typename _H = std::remove_cv_t<decltype(std::declval<T>().data())>
				//_H = std::remove_cv_t<std::iterator_traits<T>::value_type>
			>
			grain(const T& val)
			{
				// special case handling: container is of type char (not char*!)
				if (std::is_same<_H, char*>::value || std::is_same<_H, unsigned char*>::value)
				{
					type = n_bin;
					d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
					return;
				}

				type = t_list;
				d_list = std::make_unique<list_t>(val.begin(), val.end());
			};
						
			// has size_t size() and char* data()
			template<typename T, std::enable_if_t<
				!std::is_same<std::string, T>::value &&
				std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
				std::is_same<const char*, decltype(std::declval<T>().data())>::value
			>* = nullptr>
			grain(const T& val)
			{
				type = n_bin;
				d_str_bin = std::make_unique<str_bin_t>();
				d_str_bin->reserve(val.size());
				for (int idx = 0; idx < val.size(); ++idx)
					d_str_bin->push_back(*(val.data() + idx));
			};

			template <typename T, std::enable_if_t<std::is_same<const char*, T>::value>* = nullptr>
			grain(const T& val) : grain(std::string(val)) { };

			template<>
			grain(const std::string& val)
			{
				type = n_str;
				d_str_bin = std::make_unique<str_bin_t>(val.begin(), val.end());
			};

			template<>
			grain(const bool& val)
			{
				type = s_bool;
				scalar_data.d_bool = val;
			};	

			/* SUPERSEEDED BY ITERATOR INTERFACE. ONLY char POINTERS VALID FOR CONVERSION TO GENERIC BINARY
			// generalized has size_t size() and T* data()
			template<typename T, std::enable_if_t<
			std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
			std::is_pointer<decltype(std::declval<T>().data())>::value //&&
			// not iterable:
			//(	std::is_same<decltype(std::declval<T>().begin()), void>::value ||
			//	std::is_same<decltype(std::declval<T>().end()), void>::value	)
			>* = nullptr>
			grain(const T& val)
			{
			static_assert(false, "calling generalized data() size() template - not implemented yet!");
			};
			*/


			/*
			GETTERS
			attempting auto conversion here. throws if not possible!
			 - arithmetic types
			 - uchar
			 - uchar vector (binary), conversion from all types
			 - std::string
			 - bool; conversion from all types with: false if value or size of held type = 0, true otherwise

			*/
			// CATCH INCOMPATIBLE CONVERSIONS AT COMPILE TIME!
			template <typename T>
			typename std::enable_if<!std::is_arithmetic<T>::value, T>::type
			get()
			{
				static_assert(false, "you tried to convert to an incompatible type!");
			}

			template <typename T>
			typename std::enable_if<std::is_arithmetic<T>::value, T>::type
			get()
			{
				switch (type) {
				case s_int:
					return static_cast<T>(scalar_data.d_int);
					break;
				case s_fp:
					return static_cast<T>(scalar_data.d_fp);
					break;
				}

				throw std::runtime_error("cannot convert to arithmetic");

			};
			
			template<>
			unsigned char get<unsigned char>()
			{
				if (type == s_byte)
					return scalar_data.d_byte;

				throw std::runtime_error("cannot convert to string");
			};
			
			template<>
			std::vector<unsigned char> get<std::vector<unsigned char>>()
			{
				if (type == n_bin || type == n_str)
				{
					return *(d_str_bin.get());
				}

				std::vector<unsigned char> ch_vec;

				switch (type) {
				case s_int:
					for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
						ch_vec.push_back(*((char*) &(scalar_data.d_int) + i));
					return ch_vec;
					break;
				case s_fp:
					for (int i = 0; i < sizeof(scalar_data.d_int); ++i)
						ch_vec.push_back(*((char*) &(scalar_data.d_fp) + i));
					return ch_vec;
					break;
				case s_byte:
					ch_vec.push_back(*((char*) &(scalar_data.d_byte)));
					return ch_vec;
					break;
				case s_bool:
					ch_vec.push_back(*((char*) &(scalar_data.d_bool)));
					return ch_vec;
					break;
				}

				throw std::runtime_error("cannot convert to char vector");

			};

			template<>
			std::string get<std::string>()
			{
				switch (type) {
				case s_int:
					return std::to_string(scalar_data.d_int);
					break;
				case s_fp:
					return std::to_string(scalar_data.d_fp);
					break;
				case s_byte:
					return std::to_string(scalar_data.d_byte);
					break;
				case s_bool:
					return std::to_string(scalar_data.d_bool);
					break;
				case n_str:
				case n_bin:
					return std::string(d_str_bin->begin(), d_str_bin->end());
					break;
				}

				//convert map and list to empty string?
				throw std::runtime_error("cannot convert to string");

			};

			template<>
			bool get<bool>()
			{
				switch (type) {
				case s_bool:
					return scalar_data.d_bool;
					break;
				case s_int:
					return (scalar_data.d_int != 0);
					break;
				case s_fp:
					return (scalar_data.d_fp != 0);
					break;
				case s_byte:
					return (scalar_data.d_byte != 0);
					break;
				case n_str:
				case n_bin:
					return (d_str_bin->size() != 0);
					break;
				case t_list:
					return (d_list->size() != 0);
					break;
				case t_map:
					return (d_map->size() != 0);
					break;
				default:
					return false;
				}
			}

			bool is_map()
			{
				return (type == t_map);
			};

			bool is_list()
			{
				return (type == t_list);
			}

			bool is_scalar()
			{
				return (type != t_map && type != t_list);
			}

			std::size_t size()
			{
				switch (type)
				{
				case t_map:
					return d_map->size();
					break;
				case t_list:
					return d_list->size();
					break;
				default:
					return 1;
				}
			}

			milk::bite_iterator& begin(milk::bite* parent)
			{
				switch (type)
				{
				case t_map:
					return milk::bite_iterator(d_map->begin());
					break;
				case t_list:
					return milk::bite_iterator(d_list->begin());
					break;
				default:
					return milk::bite_iterator(parent);
				}
			}

			milk::bite_iterator& end(milk::bite* parent)
			{
				switch (type)
				{
				case t_map:
					return milk::bite_iterator(d_map->end());
					break;
				case t_list:
					return milk::bite_iterator(d_list->end());
					break;
				default:
					return milk::bite_iterator(parent + 1);
				}
			}

			~grain() {};
	};

}
