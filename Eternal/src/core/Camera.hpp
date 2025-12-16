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

namespace Eternal {
    class Camera {
    public:
        Camera() = default;

        Camera(Eternal::InputDispatcher* inputDispatcher);

        ~Camera() = default;

        void setOrthographicProjection(
            float left, float right, float top, float bottom, float near, float far);

        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        const glm::mat4& getProjection() const { return m_Projection; }

        glm::mat4 getView() {
            glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), m_Position);
            glm::mat4 cameraRotation = getRotation();
            return glm::inverse(cameraTranslation * cameraRotation);
        }

        glm::mat4 getRotation() {
            glm::quat pitchRotation = glm::angleAxis(m_Pitch, glm::vec3{1.f, 0.f, 0.f});
            glm::quat yawRotation = glm::angleAxis(m_Yaw, glm::vec3{0.f, -1.f, 0.f});

            return glm::toMat4(yawRotation * pitchRotation);
        }

        glm::vec3& getPosition() { return m_Position; }

        void setPosition(glm::vec3 position) { m_Position = position; }

        float getPitch() const { return m_Pitch; }

        void setPitch(float pitch) { m_Pitch = pitch; }

        float getYaw() const { return m_Yaw; }

        void setYaw(float yaw) { m_Yaw = yaw; }

        void onEvent(Event& event);

        void onUpdate(const Eternal::Timestep& timeStep);

        void resetMouseTracking();

    private:
        bool onWindowResize(const Eternal::WindowResizeEvent& event);

        bool onMouseMove(const Eternal::MouseMovedEvent& event);

        Eternal::InputDispatcher* m_InputDispatcher = nullptr;

        glm::mat4 m_Projection{1.0f};
        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_Velocity = {0.0f, 0.0f, 0.0f};

        float m_Yaw = 1.0f;
        float m_Pitch = 0.0f;
        float m_AspectRatio = 0.0f;
        float m_MouseSensitivity = 0.002f;

        // mouse state
        double m_LastX = 0.0;
        double m_LastY = 0.0;
        bool m_FirstMouse = true;
    };
}
