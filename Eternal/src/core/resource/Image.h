#pragma once

#include "core/resource/Resource.h"
#include <GLFW/glfw3.h>

namespace Eternal {
	class Image : public Resource {
	public:
		Image();
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		bool load(const std::string& path) override;
		unsigned char* getData() const { return m_Data; }
		GLFWimage getGLFWImage();
		virtual ~Image();

	private:
		int m_Width = 0;
		int m_Height = 0;
		int m_Channels = 0;
		unsigned char* m_Data = nullptr;
	};
}