#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Vortak {
    struct TransformComponent {
        glm::vec3 translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};

        static TransformComponent fromMatrix(const glm::mat4& matrix) {
            TransformComponent transform;

            glm::vec3 skew;
            glm::vec4 perspective;
            glm::quat orientation;

            glm::decompose(matrix, transform.scale, orientation, transform.translation, skew, perspective);

            transform.rotation = glm::eulerAngles(orientation);

            return transform;
        }

        glm::mat4 mat4() const {
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translation);
            glm::mat4 rotationMat = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            return translationMat * rotationMat * scaleMat;
        }

        glm::mat3 normal() const {
            return glm::transpose(glm::inverse(glm::mat3(this->mat4())));
        }
    };
}
