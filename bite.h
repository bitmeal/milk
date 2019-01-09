//#include "grain.h"

#include <memory>

namespace milk
{

	class bite
	{
		private:
			std::unique_ptr<milk::grain> grain;
		public:
			bite() : grain(nullptr) {};

			// bite is an interface for grain and holds no data other than a grain; allows construction from grain
			bite(const milk::grain& source)
			{
				grain = std::make_unique<milk::grain>(source);
			}

			bite(const bite& source) : bite(milk::grain(*(source.grain.get())))	{ };

			template<typename T>
			bite(T val)
			{
				set(val);
			};
			//bite(bite other&) {};

			template<typename T>
			void set(T val) {
				grain.release();
				grain = std::make_unique<milk::grain>(val);
			};

			
			template<typename T>
			T get() {
				return grain ? grain->get<T>() : milk::grain();
			};

			bool is_map() { return grain->is_map(); };
			bool is_list() { return grain->is_list(); };
			bool is_scalar() { return grain->is_scalar(); };

			std::size_t size() { return grain ? grain->size() : 0; };


			/*
			template<typename T>
			T begin() {
				return grain->begin<T>();
			}

			template<typename T>
			T end() {
				return grain->end<T>();
			}
			*/
			~bite() {};
		};

}

