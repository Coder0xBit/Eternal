#pragma once

#include <utils/Base.h>
#include <core/resource/Image.h>

namespace Eternal {
	struct MaterialComponent {
	public:
		MaterialComponent() = default;
		~MaterialComponent() = default;

		MaterialComponent(Image* albedoTexture) {
			m_AlbedoTexture = albedoTexture;
		}

	private:
		Image* m_AlbedoTexture = nullptr;
	};
}