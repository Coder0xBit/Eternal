#pragma once 
#include <utils/Base.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Eternal {
	struct TransformComponent  {
	public:

		TransformComponent(glm::vec3 translation = {}, glm::vec3 rot = {}, glm::vec3 scale = { 1.0f , 1.0f , 1.0f })
			: m_Translation(translation), m_Rotation(rot), m_Scale(scale) {
		}

		~TransformComponent() = default;

		glm::vec3 getTranslation() const { return m_Translation; }

		glm::vec3 getRotation() const { return m_Rotation; }

		glm::vec3 getScale() const { return m_Scale; }

		void setTranslation(glm::vec3 translation) { m_Translation = translation; }

		void setRotation(glm::vec3 rotation) { m_Rotation = rotation; }

		void setScale(glm::vec3 scale) { m_Scale = scale; }

		glm::mat4 mat4() {
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Translation);
			glm::mat4 rotation = glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

			return translation * rotation * scale;
		}

		glm::mat3 normal() {
			return glm::transpose(glm::inverse(glm::mat3(this->mat4())));
		}

	private:

		glm::vec3 m_Translation = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Scale = { 2.0f, 2.0f, 2.0f };

	};
}