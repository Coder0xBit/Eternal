#include "core/Camera.h"
#include "core/event/EventDispatcher.h"

namespace Eternal {
    Camera::Camera(Eternal::InputDispatcher* inputDispatcher) : mInputDispatcher(inputDispatcher) {
    }

    void Eternal::Camera::setOrthographicProjection(float left, float right, float top, float bottom, float nearPlane,
                                                    float farPlane) {
        mProjection = glm::mat4{1.0f};
        mProjection[0][0] = 2.f / (right - left);
        mProjection[1][1] = 2.f / (bottom - top);
        mProjection[2][2] = 1.f / (farPlane - nearPlane);
        mProjection[3][0] = -(right + left) / (right - left);
        mProjection[3][1] = -(bottom + top) / (bottom - top);
        mProjection[3][2] = -nearPlane / (farPlane - nearPlane);
    }

    void Eternal::Camera::setPerspectiveProjection(float fovy, float aspect, float nearPlane, float farPlane) {
        ETERNAL_ASSERT(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f,
                       "something wrong in setPerspectiveProjection(...)");
        glm::mat4 projection = glm::perspective(fovy, aspect, nearPlane, farPlane);
        projection[1][1] *= -1; // Invert Y axis Since Vulkan uses a different coordinate system
        mProjection = projection;
    }

    void Camera::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<Eternal::WindowResizeEvent>(ETERNAL_BIND_EVENT_FN(Camera::onWindowResize));
        dispatcher.dispatch<Eternal::MouseMovedEvent>(ETERNAL_BIND_EVENT_FN(Camera::onMouseMove));
    }

    void Camera::resetMouseTracking() {
        mFirstMouse = true;
    }

    void Camera::onUpdate(const Eternal::Timestep& timeStep) {
        float velocity = 10.0f * timeStep.seconds();

        glm::mat4 rotation = getRotation();
        glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0, 0, -1, 0)));
        glm::vec3 right = glm::normalize(glm::vec3(rotation * glm::vec4(1, 0, 0, 0)));
        glm::vec3 up = glm::normalize(glm::vec3(rotation * glm::vec4(0, 1, 0, 0)));

        if (mInputDispatcher->isKeyPressed(Key::W))
            mPosition += forward * velocity;
        if (mInputDispatcher->isKeyPressed(Key::S))
            mPosition -= forward * velocity;
        if (mInputDispatcher->isKeyPressed(Key::A))
            mPosition -= right * velocity;
        if (mInputDispatcher->isKeyPressed(Key::D))
            mPosition += right * velocity;
        if (mInputDispatcher->isKeyPressed(Key::Space))
            mPosition += up * velocity;
        if (mInputDispatcher->isKeyPressed(Key::LeftShift))
            mPosition -= up * velocity;
    }

    bool Camera::onWindowResize(const Eternal::WindowResizeEvent& event) {
        float aspectRatio = static_cast<float>(event.getWidth()) / static_cast<float>(event.getHeight());
        setPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);
        return true;
    }

    bool Camera::onMouseMove(const Eternal::MouseMovedEvent& event) {
        double xpos = event.GetX();
        double ypos = event.GetY();

        if (mFirstMouse) {
            mLastX = xpos;
            mLastY = ypos;
            mFirstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - mLastX);
        float yoffset = static_cast<float>(mLastY - ypos);
        mLastX = xpos;
        mLastY = ypos;

        xoffset *= mMouseSensitivity;
        yoffset *= mMouseSensitivity;

        mYaw += xoffset;
        mPitch += yoffset;

        float pitch = glm::degrees(mPitch);
        if (pitch > 89.0f) mPitch = glm::radians(89.0f);
        if (pitch < -89.0f) mPitch = glm::radians(-89.0f);
        return true;
    }
}
