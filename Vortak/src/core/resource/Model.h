#pragma once

#include "core/resource/Image.h"
#include "core/resource/Mesh.h"
#include "core/resource/Resource.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Vortak {
    struct ModelNode {
        std::string name;
        std::vector<uint32_t> meshIndices;
        std::unique_ptr<uint32_t> textureIndices;
        glm::mat4 localTransform;
        std::vector<std::unique_ptr<ModelNode>> children;
    };

    class Model : public Resource<Model> {
    public:
        Model();

        Model(const Model& other) = delete;

        Model& operator=(const Model&) = delete;

        bool load(const std::string& path) override;

        ~Model() override;

        const std::vector<std::unique_ptr<Mesh>>& getMeshes() const { return mMeshes; }

        const std::unique_ptr<ModelNode>& getRootNode() const { return mRoot; }

    private :
        std::unique_ptr<ModelNode> processNode(aiNode* node, const aiScene* scene);

        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

    private:
        std::unique_ptr<ModelNode> mRoot;
        std::string mModelDirectory;

        std::vector<std::unique_ptr<Mesh>> mMeshes;
        std::vector<std::unique_ptr<Image>> mTextures;
    };
}
