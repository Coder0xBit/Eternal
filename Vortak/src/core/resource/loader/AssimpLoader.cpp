#include "core/resource/loader/AssimpLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Vortak {
    std::unique_ptr<Model> AssimpLoader::load(const std::filesystem::path& path) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(
            path.string(),
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
            aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

        if (!scene || !scene->mRootNode) {
            Logger::Error("Failed to import model: {}", importer.GetErrorString());
            return nullptr;
        }

        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.reserve(scene->mNumMeshes);

        // Convert meshes
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
            aiMesh* aiMesh = scene->mMeshes[meshIndex];

            std::vector<Vortak::Vertex> vertices;
            std::vector<uint32_t> indices;

            vertices.reserve(aiMesh->mNumVertices);

            for (uint32_t vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++) {
                Vertex vertex{};

                vertex.position =
                {
                    aiMesh->mVertices[vertexIndex].x,
                    aiMesh->mVertices[vertexIndex].y,
                    aiMesh->mVertices[vertexIndex].z
                };

                if (aiMesh->HasNormals()) {
                    vertex.normal =
                    {
                        aiMesh->mNormals[vertexIndex].x,
                        aiMesh->mNormals[vertexIndex].y,
                        aiMesh->mNormals[vertexIndex].z
                    };
                }

                if (aiMesh->HasTextureCoords(0)) {
                    vertex.uv =
                    {
                        aiMesh->mTextureCoords[0][vertexIndex].x,
                        aiMesh->mTextureCoords[0][vertexIndex].y
                    };
                }

                // if (aiMesh->HasTangentsAndBitangents()) {
                //     vertex.tangent =
                //     {
                //         aiMesh->mTangents[vertexIndex].x,
                //         aiMesh->mTangents[vertexIndex].y,
                //         aiMesh->mTangents[vertexIndex].z
                //     };
                //
                //     vertex.bitangent =
                //     {
                //         aiMesh->mBitangents[vertexIndex].x,
                //         aiMesh->mBitangents[vertexIndex].y,
                //         aiMesh->mBitangents[vertexIndex].z
                //     };
                // }

                vertices.push_back(vertex);
            }

            indices.reserve(aiMesh->mNumFaces * 3);

            for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++) {
                const aiFace& face = aiMesh->mFaces[faceIndex];

                for (uint32_t i = 0; i < face.mNumIndices; i++) {
                    indices.push_back(face.mIndices[i]);
                }
            }

            auto mesh = std::make_unique<Mesh>(vertices, indices);
            meshes.emplace_back(std::move(mesh));
        }

        std::unique_ptr<ModelNode> rootNode = std::make_unique<ModelNode>();
        processNode(scene->mRootNode, scene, rootNode.get());

        auto model = std::make_unique<Model>();
        model->root = std::move(rootNode);
        model->meshes = std::move(meshes);

        return model;
    }

    void AssimpLoader::processNode(aiNode* node, const aiScene* scene, ModelNode* modelNode) {
        modelNode->name = node->mName.C_Str();
        modelNode->localTransform = AssimpUtils::to_glm(node->mTransformation);

        // Meshes
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            modelNode->meshIndices.push_back(node->mMeshes[i]);
        }

        modelNode->children.reserve(node->mNumChildren);

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            auto childNode = std::make_unique<ModelNode>();
            processNode(node->mChildren[i], scene, childNode.get());
            modelNode->children.emplace_back(std::move(childNode));
        }
    }
}
