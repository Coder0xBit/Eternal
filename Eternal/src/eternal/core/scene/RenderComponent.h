#pragma once

#include <eternal/utils/Base.h>
#include <eternal/core/graphics/Vertex.h>

namespace Eternal {
	struct RenderComponent {
	public:

		RenderComponent() = default;

		RenderComponent(std::vector<Eternal::Vertex> vertices, std::vector<uint32_t> indices)
			: m_Vertices(vertices), m_Indices(indices) {
		}

		~RenderComponent() = default;

		const std::vector<Eternal::Vertex>& getVertices() const { return m_Vertices; }

		const std::vector<uint32_t>& getIndices() const { return m_Indices; }

	private:

		std::vector<Eternal::Vertex> m_Vertices;

		std::vector<uint32_t> m_Indices;

	};
}