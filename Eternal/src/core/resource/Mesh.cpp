#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <utils/TinyObjLoader.h>

namespace std {
	template <>
	struct hash<Eternal::Vertex>
	{
		size_t operator()(Eternal::Vertex const& vertex) const
		{
			size_t seed = 0;
			Eternal::hashCombine(seed,
				vertex.position.x, vertex.position.y, vertex.position.z,
				vertex.color.x, vertex.color.y, vertex.color.z,
				vertex.normal.x, vertex.normal.y, vertex.normal.z,
				vertex.uv.x, vertex.uv.y);
			return seed;
		}
	};
}

namespace Eternal {

	Mesh::Mesh() {}

	bool Mesh::load(const std::string& path) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			ETERNAL_ASSERT_LOG(true, warn + err);
			return false;
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					if (attrib.colors.empty()) {
						vertex.color = { 1.0f, 1.0f, 1.0f };
					}
					else {
						vertex.color = {
							attrib.colors[3 * index.vertex_index + 0],
							attrib.colors[3 * index.vertex_index + 1],
							attrib.colors[3 * index.vertex_index + 2],
						};
					}
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
					m_Vertices.push_back(vertex);
				}
				m_Indices.push_back(uniqueVertices[vertex]);
			}
		}

		return true;
	}

	Mesh::~Mesh() {
		m_Vertices.clear();
		m_Indices.clear();
	}
}
