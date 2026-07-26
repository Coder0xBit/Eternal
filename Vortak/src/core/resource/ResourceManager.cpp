#include "core/resource/ResourceManager.h"

namespace Vortak {
    template <>
    ResourceCache<Mesh>& ResourceManager::cache<Mesh>() {
        return mMeshes;
    }

    template <>
    ResourceCache<Model>& ResourceManager::cache<Model>() {
        return mModels;
    }

    template <>
    ResourceCache<Image>& ResourceManager::cache<Image>() {
        return mImages;
    }

    template <>
    ResourceCache<ShaderProgram>& ResourceManager::cache<ShaderProgram>() {
        return mShaders;
    }
}
