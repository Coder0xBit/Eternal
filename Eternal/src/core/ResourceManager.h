#include <utils/Base.h>

namespace Eternal
{
	class Resource {
	public:
		Resource() = default;
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

		virtual ~Resource() = default;

	protected:
		std::string m_Name;
	};

	class ResourceManager {
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
	};
}