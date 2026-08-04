#include "utils/Base.h"

#include "core/resource/Model.h"
#include "core/resource/Image.h"
#include "core/resource/ShaderProgram.h"

#include "core/resource/loader/AssimpLoader.h"
#include "core/resource/loader/ImageLoader.h"
#include "core/resource/loader/ShaderLoader.h"

namespace Vortak {
    template <typename T>
    struct LoaderTraits;

    template <>
    struct LoaderTraits<Model> {
        using type = AssimpLoader;
    };

    template <>
    struct LoaderTraits<Image> {
        using type = ImageLoader;
    };

    template <>
    struct LoaderTraits<ShaderProgram> {
        using type = ShaderLoader;
    };
}
