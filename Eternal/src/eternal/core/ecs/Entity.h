#pragma once
#include <eternal/utils/Base.h>

namespace Eternal {
	class Entity {
	public:
		explicit Entity(uint32_t id) : m_Id(id) {};

		uint32_t getId() const { return m_Id; }

		Entity(const Entity&) = delete;

		Entity(Entity&&) = default;

		Entity& operator=(const Entity&) = delete;

		Entity& operator=(Entity&&) = default;

		bool operator==(const Entity& other) const { return m_Id == other.m_Id; }

		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		uint32_t m_Id = 0;
	};
}