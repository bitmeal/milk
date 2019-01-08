//#include "grain.h"

#include <memory>

namespace milk {

	class bite {
		private:
			std::unique_ptr<milk::grain> grain;
		public:
			bite() : grain(nullptr) {};

			//bite(bite other&) {};

			template<typename T>
			void set(T val) {
				grain.release();
				grain = std::make_unique<milk::grain>(val);
			};
			
			template<typename T>
			T get() {
				return grain->get<T>();
			};
			
			~bite() {};
		};

}

