#include "Model.h"


namespace Vortak {
    Model::Model() {
    }

    bool Model::load(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Vortak::Logger::Error("assimp had some issues :: {}", importer.GetErrorString());
            return false;
        }

        mModelDirectory = path.substr(0, path.find_last_of('/'));

        mMeshes.reserve(scene->mNumMeshes);
        for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            mMeshes.push_back(processMesh(mesh, scene));
        }

        mRoot = processNode(scene->mRootNode, scene);
        return true;
    }

    Model::~Model() {
    }

    std::unique_ptr<ModelNode> Model::processNode(aiNode* node, const aiScene* scene) {
        auto modelNode = std::make_unique<ModelNode>();

        // Name
        modelNode->name = node->mName.C_Str();

        // Transform
        const aiMatrix4x4& t = node->mTransformation;

        modelNode->localTransform = glm::mat4(
            t.a1, t.b1, t.c1, t.d1,
            t.a2, t.b2, t.c2, t.d2,
            t.a3, t.b3, t.c3, t.d3,
            t.a4, t.b4, t.c4, t.d4
        );

        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            uint32_t meshIndex = node->mMeshes[i];
            modelNode->meshIndices.push_back(meshIndex);
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i) {
            modelNode->children.push_back(
                processNode(node->mChildren[i], scene)
            );
        }

        return modelNode;
    }

    std::unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vortak::Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        std::vector<uint32_t> indices;
        uint32_t indexCount = 0;
        for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
            indexCount += mesh->mFaces[i].mNumIndices;
        }
        indices.reserve(indexCount);

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.uv = vec;
            } else {
                vertex.uv = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_unique<Mesh>(vertices, indices);
    }
}
