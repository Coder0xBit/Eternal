#pragma once

#include <utils/Base.h>

namespace Eternal {
	struct NameComponent {
	public:
		NameComponent() = default;
		NameComponent(std::string name) : m_Name(name) {}
		~NameComponent() = default;
		const char* getName() { return m_Name.c_str(); }
		void setName(const std::string& name) { m_Name = name; }
		void setName(const char* name) { m_Name = name; }

	private:
		std::string m_Name = "Entity";
	};
}