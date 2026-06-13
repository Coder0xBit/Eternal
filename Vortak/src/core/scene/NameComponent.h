#pragma once

#include "utils/Base.h"

namespace Vortak {
	struct NameComponent {
	public:
		NameComponent() = default;
		NameComponent(std::string name) : mName(name) {}
		~NameComponent() = default;
		const char* getName() { return mName.c_str(); }
		void setName(const std::string& name) { mName = name; }
		void setName(const char* name) { mName = name; }

	private:
		std::string mName = "Entity";
	};
}