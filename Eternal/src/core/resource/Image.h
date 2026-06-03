#pragma once

#include "core/resource/Resource.h"

#include <GLFW/glfw3.h>

namespace Eternal {

	constexpr uint32_t DESIRED_IMAGE_CHANNELS = 4; // RGBA				

	class Image : public Resource {
	public:
		Image();
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		bool load(const std::string& path) override;
		unsigned char* getData() const { return mData; }
		GLFWimage getGLFWImage();
		int getWidth() const { return mWidth; }
		int getHeight() const { return mHeight; }
		int getPixelCount() const { return mWidth * mHeight; }
		int getChannels() const { return mChannels; }
		virtual ~Image();

	private:
		int mWidth = 0;
		int mHeight = 0;
		int mChannels = 0;
		unsigned char* mData = nullptr;

	};
}