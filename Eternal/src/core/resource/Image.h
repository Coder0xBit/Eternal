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
		unsigned char* getData() const { return m_Data; }
		GLFWimage getGLFWImage();
		int getWidth() const { return m_Width; }
		int getHeight() const { return m_Height; }
		int getPixelCount() const { return m_Width * m_Height; }
		int getChannels() const { return m_Channels; }
		virtual ~Image();

	private:
		int m_Width = 0;
		int m_Height = 0;
		int m_Channels = 0;
		unsigned char* m_Data = nullptr;

	};
}