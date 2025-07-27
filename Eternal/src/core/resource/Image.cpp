#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Eternal
{
	Image::Image() {}

	bool Image::load(const std::string& path) {
		stbi_set_flip_vertically_on_load(true);
		m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);

		if (!m_Data)
		{
			Eternal::Logger::Error("Failed to load icon, from this path {}", path);
			return false;
		}

		if (m_Channels < DESIRED_IMAGE_CHANNELS)
		{
			Eternal::Logger::Warn("Image loaded from path {} has less than 4 channels, reverting the channels to {}", path, DESIRED_IMAGE_CHANNELS);
			m_Channels = DESIRED_IMAGE_CHANNELS;
		}

		return true;
	}

	GLFWimage Image::getGLFWImage()
	{
		GLFWimage glfwImage;
		glfwImage.width = m_Width;
		glfwImage.height = m_Height;
		glfwImage.pixels = m_Data;
		return glfwImage;
	}

	Image::~Image() {
		stbi_image_free(m_Data);
	}
}
