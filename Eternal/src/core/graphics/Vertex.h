#pragma once

#include <glm/glm.hpp>
#include <utils/Base.h>

#include <vulkan/vulkan.hpp>

namespace Eternal {
	struct Vertex {
	public:
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec2 uv;

	public:
		Vertex() = default;
		Vertex(float x, float y, float z) : position(x, y, z) {}
		Vertex(glm::vec3 pos, glm::vec3 color) : position(pos), color(color) {}
		Vertex(float value) : position(value, value, value) {}

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color;
		}

		static std::vector<vk::VertexInputBindingDescription> getBindingDescription() {
			vk::VertexInputBindingDescription bindingDesc = vk::VertexInputBindingDescription()
				.setBinding(0)
				.setStride(sizeof(Eternal::Vertex))
				.setInputRate(vk::VertexInputRate::eVertex);

			return { bindingDesc };
		}

		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescription() {
			std::vector<vk::VertexInputAttributeDescription> attributeDescs;
			vk::VertexInputAttributeDescription positionAttribDesc = vk::VertexInputAttributeDescription()
				.setLocation(0)
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(offsetof(Vertex, position));
			attributeDescs.push_back(positionAttribDesc);

			vk::VertexInputAttributeDescription colorAttribDesc = vk::VertexInputAttributeDescription()
				.setLocation(1)
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(offsetof(Vertex, color));
			attributeDescs.push_back(colorAttribDesc);

			vk::VertexInputAttributeDescription normalAttribDesc = vk::VertexInputAttributeDescription()
				.setLocation(2)
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(offsetof(Vertex, normal));
			attributeDescs.push_back(normalAttribDesc);

			vk::VertexInputAttributeDescription uvAttribDesc = vk::VertexInputAttributeDescription()
				.setLocation(3)
				.setBinding(0)
				.setFormat(vk::Format::eR32G32Sfloat)
				.setOffset(offsetof(Vertex, uv));
			attributeDescs.push_back(uvAttribDesc);

			return attributeDescs;
		}
	};
}