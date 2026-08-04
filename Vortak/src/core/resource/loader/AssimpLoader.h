#pragma once

#include "core/resource/loader/AssetLoader.h"
#include "core/resource/Model.h"

#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Vortak {
    namespace AssimpUtils {
        inline glm::mat4 to_glm(const aiMatrix4x4& matrix) {
            return glm::transpose(glm::make_mat4(&matrix.a1));
        }
    }

    class AssimpLoader : public AssetLoader<Model> {
    public:
        AssimpLoader() = default;

        std::unique_ptr<Model> load(const std::filesystem::path& path) override;

    private :
        static void processNode(aiNode* node, const aiScene* scene, ModelNode* modelNode);
    };
}
