#include "core/resource/Image.h"
#include <stb/stb_image.h>

namespace Vortak
{
	Image::Image() {}

	bool Image::load(const std::string& path) {
		/*stbi_set_flip_vertically_on_load(true);*/
		mData = stbi_load(path.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);

		if (!mData)
		{
			Vortak::Logger::Error("Failed to load icon, from this path {}", path);
			return false;
		}

		if (mChannels < DESIRED_IMAGE_CHANNELS)
		{
			Vortak::Logger::Warn("Image loaded from path {} has less than 4 channels, reverting the channels to {}", path, DESIRED_IMAGE_CHANNELS);
			mChannels = DESIRED_IMAGE_CHANNELS;
		}

		return true;
	}

	GLFWimage Image::getGLFWImage()
	{
		GLFWimage glfwImage;
		glfwImage.width = mWidth;
		glfwImage.height = mHeight;
		glfwImage.pixels = mData;
		return glfwImage;
	}

	Image::~Image() {
		stbi_image_free(mData);
	}
}
