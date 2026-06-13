#pragma once

#include "utils/Base.h"
#include "core/input/InputDispatcher.h"
#include "core/event/Event.h"
#include "core/event/MouseEvents.h"
#include "core/event/WindowEvent.h"
#include "core/graphics/Timer.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>

namespace Vortak {
    class Camera {
    public:
        Camera() = default;

        Camera(Vortak::InputDispatcher* inputDispatcher);

        ~Camera() = default;

        void setOrthographicProjection(
            float left, float right, float top, float bottom, float near, float far);

        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        const glm::mat4& getProjection() const { return mProjection; }

        glm::mat4 getView() {
            glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), mPosition);
            glm::mat4 cameraRotation = getRotation();
            return glm::inverse(cameraTranslation * cameraRotation);
        }

        glm::mat4 getRotation() {
            glm::quat pitchRotation = glm::angleAxis(mPitch, glm::vec3{1.f, 0.f, 0.f});
            glm::quat yawRotation = glm::angleAxis(mYaw, glm::vec3{0.f, -1.f, 0.f});

            return glm::toMat4(yawRotation * pitchRotation);
        }

        glm::vec3& getPosition() { return mPosition; }

        void setPosition(glm::vec3 position) { mPosition = position; }

        float getPitch() const { return mPitch; }

        void setPitch(float pitch) { mPitch = pitch; }

        float getYaw() const { return mYaw; }

        void setYaw(float yaw) { mYaw = yaw; }

        void onEvent(Event& event);

        void onUpdate(const Vortak::Timestep& timeStep);

        void resetMouseTracking();

    private:
        bool onWindowResize(const Vortak::WindowResizeEvent& event);

        bool onMouseMove(const Vortak::MouseMovedEvent& event);

        Vortak::InputDispatcher* mInputDispatcher = nullptr;

        glm::mat4 mProjection{1.0f};
        glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 mVelocity = {0.0f, 0.0f, 0.0f};

        float mYaw = 1.0f;
        float mPitch = 0.0f;
        float mAspectRatio = 0.0f;
        float mMouseSensitivity = 0.002f;

        // mouse state
        double mLastX = 0.0;
        double mLastY = 0.0;
        bool mFirstMouse = true;
    };
}
