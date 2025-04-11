#pragma once

#include <eternal/utils/Base.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Eternal {
	class Camera {
	public:
		Camera() = default;

		~Camera() = default;

		void setOrthographicProjection(
			float left, float right, float top, float bottom, float near, float far);

		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		const glm::mat4& getProjection() const { return projectionMatrix; }

	private:
		glm::mat4 projectionMatrix{ 1.f };
	};
}