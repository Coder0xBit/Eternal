#pragma once

#include "core/resource/Image.h"
#include "core/resource/Mesh.h"
#include "core/resource/Resource.h"

namespace Vortak {
    struct ModelNode {
        std::string name;
        std::vector<uint32_t> meshIndices;
        std::unique_ptr<uint32_t> textureIndices;
        glm::mat4 localTransform;
        std::vector<std::unique_ptr<ModelNode>> children;
    };

    struct Model : public Resource<Model> {
        std::unique_ptr<ModelNode> root;

        std::vector<std::unique_ptr<Mesh>> meshes;
    };
}
