#pragma once
#include <glm/glm.hpp>
#include <eternal/utils/Base.h>

#include <vulkan/vulkan.hpp>

namespace Eternal {
	struct Vertex
	{
		glm::vec3 position;

		Vertex() = default;

		Vertex(float x, float y, float z) : position(x, y, z) {}

		static std::vector<vk::VertexInputBindingDescription> getBindingDescription()
		{
			vk::VertexInputBindingDescription bindingDesc = vk::VertexInputBindingDescription()
				.setBinding(0)
				.setStride(sizeof(Eternal::Vertex))
				.setInputRate(vk::VertexInputRate::eVertex);

			return { bindingDesc };
		}

		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescription()
		{
			vk::VertexInputAttributeDescription attributeDesc = vk::VertexInputAttributeDescription()
				.setLocation(0)
				.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(0);

			return { attributeDesc };
		}
	};
}