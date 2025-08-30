#include "Camera.hpp"

namespace Eternal {
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

    void Camera::onUpdate(GLFWwindow* window) {
        processMouseMovement(window);
        processKeyboad(window);
    }

    void Camera::processMouseMovement(GLFWwindow* window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (m_FirstMouse) {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - m_LastX);
        float yoffset = static_cast<float>(m_LastY - ypos); // reversed since y-coordinates go bottom to top
        m_LastX = xpos;
        m_LastY = ypos;

        xoffset *= m_MouseSensitivity;
        yoffset *= m_MouseSensitivity;

        m_Yaw += xoffset;
        m_Pitch += yoffset;

        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;
    }

    void Camera::processKeyboad(GLFWwindow* window) {
        float velocity = 0.1f;

        // Forward vector from yaw/pitch
        glm::mat4 rotation = getRotation();
        glm::vec3 forward  = glm::normalize(glm::vec3(rotation * glm::vec4(0, 0, -1, 0)));
        glm::vec3 right    = glm::normalize(glm::vec3(rotation * glm::vec4(1, 0, 0, 0)));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Position += forward * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Position -= forward * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Position += right * velocity;
    }
}
