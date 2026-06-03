#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Eternal {
    struct TransformComponent {
        TransformComponent(glm::vec3 translation = {}, glm::vec3 rot = {}, glm::vec3 scale = {1.0f, 1.0f, 1.0f})
            : mTranslation(translation), mRotation(rot), mScale(scale) {
        }

        ~TransformComponent() = default;

        glm::vec3 getTranslation() const { return mTranslation; }
        glm::vec3 getRotation() const { return mRotation; }
        glm::vec3 getScale() const { return mScale; }
        void setTranslation(glm::vec3 translation) { mTranslation = translation; }
        void setRotation(glm::vec3 rotation) { mRotation = rotation; }
        void setScale(glm::vec3 scale) { mScale = scale; }

        glm::mat4 mat4() {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), mTranslation);
            glm::mat4 rotation = glm::yawPitchRoll(mRotation.y, mRotation.x, mRotation.z);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), mScale);

            return translation * rotation * scale;
        }

        glm::mat3 normal() {
            return glm::transpose(glm::inverse(glm::mat3(this->mat4())));
        }

    private:
        glm::vec3 mTranslation = {0.0f, 0.0f, 0.0f};
        glm::vec3 mRotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 mScale = {1.0f, 1.0f, 1.0f};
    };
}
