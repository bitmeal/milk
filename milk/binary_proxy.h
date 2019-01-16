
namespace milk
{

	struct binary_proxy
	{
	private:
		const unsigned char* _data;
		std::size_t _size;

	public:
		binary_proxy() : _data(nullptr), _size(0) {};
		binary_proxy(const unsigned char* data, std::size_t size) : _data(data), _size(size) {};

		const unsigned char* data() const { return _data; };
		std::size_t size() const { return _size; };
	};

}