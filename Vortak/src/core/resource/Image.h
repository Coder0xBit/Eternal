#pragma once

#include "core/resource/Resource.h"

#include <GLFW/glfw3.h>

namespace Vortak {
    constexpr uint32_t DESIRED_IMAGE_CHANNELS = 4; // RGBA

    struct Image : Resource<Image> {
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = nullptr;

        int getPixelCount() const { return width * height; }

        GLFWimage getGLFWImage() {
            GLFWimage glfwImage;
            glfwImage.width = width;
            glfwImage.height = height;
            glfwImage.pixels = data;
            return glfwImage;
        }
    };
}
