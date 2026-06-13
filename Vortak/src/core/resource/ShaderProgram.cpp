#include "core/resource/ShaderProgram.h"

namespace Vortak {
	ShaderProgram::ShaderProgram() = default;

	bool ShaderProgram::load(const std::string& path) {
		if (path.empty()) {
			Vortak::Logger::Error("ShaderProgram::load() path is empty");
			return false;
		}

		std::ifstream stream(path, std::ios::binary);

		if (!stream) {
			Vortak::Logger::Error("ShaderProgram::load() failed");
			return false;
		}

		stream.seekg(0, std::ios_base::end);
		std::streampos size = stream.tellg();
		stream.seekg(0, std::ios_base::beg);

		mBlob.resize(size);
		stream.read(mBlob.data(), size);
		stream.close();

		return true;
	}

	ShaderProgram::~ShaderProgram() {
		mBlob.clear();
	}
}
