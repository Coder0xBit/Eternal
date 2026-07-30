#pragma once

#include "core/resource/importer/AssetImporter.h"
#include "core/resource/Model.h"

#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>

namespace Vortak {
    namespace AssimpUtils {
        inline glm::mat4 to_glm(const aiMatrix4x4& matrix) {
            return glm::transpose(glm::make_mat4(&matrix.a1));
        }
    }

    class AssimpImporter : public AssetImporter<Model> {
    public:
        AssimpImporter() = default;

        std::unique_ptr<Model> import(const std::filesystem::path& path) override;

    private :
        static void processNode(aiNode* node, const aiScene* scene, ModelNode* modelNode);
    };
}
