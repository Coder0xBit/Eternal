#include "Camera.hpp"

namespace Eternal {
	void Eternal::Camera::setOrthographicProjection(float left, float right, float top, float bottom, float nearPlane, float farPlane) {
		projectionMatrix = glm::mat4{ 1.0f };
		projectionMatrix[0][0] = 2.f / (right - left);
		projectionMatrix[1][1] = 2.f / (bottom - top);
		projectionMatrix[2][2] = 1.f / (farPlane - nearPlane);
		projectionMatrix[3][0] = -(right + left) / (right - left);
		projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -nearPlane / (farPlane - nearPlane);
	}

	void Eternal::Camera::setPerspectiveProjection(float fovy, float aspect, float nearPlane, float farPlane) {
		ETERNAL_ASSERT(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "something wrong in setPerspectiveProjection(...)");
		glm::mat4 projection = glm::perspective(fovy, aspect, nearPlane, farPlane);
		projection[1][1] *= -1; // Invert Y axis Since Vulkan uses a different coordinate system
		projectionMatrix = projection;
	}
}

