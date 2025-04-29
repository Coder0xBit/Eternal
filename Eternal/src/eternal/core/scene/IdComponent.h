#pragma once
#include <eternal/utils/Base.h>
#include <eternal/utils/UUID.h>

namespace Eternal
{
	struct IdComponent
	{
	public:

		IdComponent() = default;

		IdComponent(const UUID& id) : m_Id(id) {}

		IdComponent(const IdComponent&) = default;

		UUID getId() { return m_Id; }

	private:

		UUID m_Id;
	};
}