#include "core/Camera.hpp"
#include "core/event/EventDispatcher.h"

namespace Eternal {
    Camera::Camera(Eternal::InputDispatcher* inputDispatcher) : m_InputDispatcher(inputDispatcher) {
    }

    void Eternal::Camera::setOrthographicProjection(float left, float right, float top, float bottom, float nearPlane,
                                                    float farPlane) {
        m_Projection = glm::mat4{1.0f};
        m_Projection[0][0] = 2.f / (right - left);
        m_Projection[1][1] = 2.f / (bottom - top);
        m_Projection[2][2] = 1.f / (farPlane - nearPlane);
        m_Projection[3][0] = -(right + left) / (right - left);
        m_Projection[3][1] = -(bottom + top) / (bottom - top);
        m_Projection[3][2] = -nearPlane / (farPlane - nearPlane);
    }

    void Eternal::Camera::setPerspectiveProjection(float fovy, float aspect, float nearPlane, float farPlane) {
        ETERNAL_ASSERT(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f,
                       "something wrong in setPerspectiveProjection(...)");
        glm::mat4 projection = glm::perspective(fovy, aspect, nearPlane, farPlane);
        projection[1][1] *= -1; // Invert Y axis Since Vulkan uses a different coordinate system
        m_Projection = projection;
    }

    void Camera::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<Eternal::WindowResizeEvent>(ETERNAL_BIND_EVENT_FN(Camera::onWindowResize));
        dispatcher.dispatch<Eternal::MouseMovedEvent>(ETERNAL_BIND_EVENT_FN(Camera::onMouseMove));
    }

    void Camera::resetMouseTracking() {
        m_FirstMouse = true;
    }

    void Camera::onUpdate(const Eternal::Timestep& timeStep) {
        float velocity = 10.0f * timeStep.seconds();

        glm::mat4 rotation = getRotation();
        glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0, 0, -1, 0)));
        glm::vec3 right = glm::normalize(glm::vec3(rotation * glm::vec4(1, 0, 0, 0)));
        glm::vec3 up = glm::normalize(glm::vec3(rotation * glm::vec4(0, 1, 0, 0)));

        if (m_InputDispatcher->isKeyPressed(Key::W))
            m_Position += forward * velocity;
        if (m_InputDispatcher->isKeyPressed(Key::S))
            m_Position -= forward * velocity;
        if (m_InputDispatcher->isKeyPressed(Key::A))
            m_Position -= right * velocity;
        if (m_InputDispatcher->isKeyPressed(Key::D))
            m_Position += right * velocity;
        if (m_InputDispatcher->isKeyPressed(Key::Space))
            m_Position += up * velocity;
        if (m_InputDispatcher->isKeyPressed(Key::LeftShift))
            m_Position -= up * velocity;
    }

    bool Camera::onWindowResize(const Eternal::WindowResizeEvent& event) {
        float aspectRatio = static_cast<float>(event.getWidth()) / static_cast<float>(event.getHeight());
        setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);
        return true;
    }

    bool Camera::onMouseMove(const Eternal::MouseMovedEvent& event) {
        double xpos = event.GetX();
        double ypos = event.GetY();

        if (m_FirstMouse) {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - m_LastX);
        float yoffset = static_cast<float>(m_LastY - ypos);
        m_LastX = xpos;
        m_LastY = ypos;

        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        float pitch = glm::degrees(m_Pitch);
        if (pitch > 89.0f) m_Pitch = glm::radians(89.0f);
        if (pitch < -89.0f) m_Pitch = glm::radians(-89.0f);
        return true;
    }
}
