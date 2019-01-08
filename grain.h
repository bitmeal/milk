#include <type_traits>

#include <memory>

#include <string>
#include <array>
#include <vector>
#include <map>

#include <exception>
#include <typeinfo>

//#include "bite.h"

namespace milk {

	class grain {
		private:
			union t_scalar_data {
				int64_t			d_int;
				double_t		d_fp;
				unsigned char	d_byte;
				bool			d_bool;
			};
			t_scalar_data scalar_data;
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
			grain::t_type type;
			
			char bin_ext;

			int l_str_bin;
			std::unique_ptr<std::vector<unsigned char>> d_str_bin;
			std::unique_ptr<std::map<std::string, milk::bite>> d_map;
			std::unique_ptr<std::vector<milk::bite>> d_list;
			

		public:
			/*
			CONSTRUCTORS - SETTERS
			 - integral types + char/uchar
			 - floating point types
			 - std::string
			 //- string from char*
			 - uchar vector (binary)
			 - bool
			*/
			grain() {};
			
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

			template <typename T, std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
			grain(const T& val)
			{
					type = s_fp;
					scalar_data.d_fp = val;
					return;
			};
			
			/*
			template<>
			grain(const std::vector<unsigned char>& val)
			{
				type = n_bin;
				d_str_bin = std::make_unique<std::vector<unsigned char>>(val);
			};
			*/

			// generalized has size_t size() and T* data()
			template<typename T, std::enable_if_t<
				std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
				std::is_pointer<decltype(std::declval<T>().data())>::value
			>* = nullptr>
			grain(const T& val)
			{
				static_assert(false, "calling generalized data() size() template - not implemented yet!");
			};

			// has size_t size() and char* data()
			template<typename T, std::enable_if_t<
				std::is_same<std::size_t, decltype(std::declval<T>().size())>::value &&
				std::is_same<char*, decltype(std::declval<T>().data())>::value
			>* = nullptr>
			grain(const T& val)
			{
				type = n_str;
				d_str_bin = std::make_unique<std::vector<unsigned char>>();
				d_str_bin->reserve(val.size());
				for (int idx = 0; idx < val.size(); ++i)
					d_str_bin->push_back(val.data() + i);
			};

						
			template<>
			grain(const bool& val)
			{
				type = s_bool;
				scalar_data.d_bool = val;
			};

			template<>
			grain(const std::string& val)
			{
				type = n_str;
				d_str_bin = std::make_unique<std::vector<unsigned char>>(val.begin(), val.end());
			};
			
			template <typename T, std::enable_if_t<std::is_same<char*, T>::value>* = nullptr>
			grain(const T& val) : grain(std::string(val)) { };

			/*
			GETTERS
			attempting auto conversion here. throws if not possible!
			 - arithmetic types
			 - uchar
			 - uchar vector (binary), conversion from all types
			 - std::string
			 - bool; false if value or size of any held type = 0, true otherwise

			*/
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



			~grain() {};
	};

}

