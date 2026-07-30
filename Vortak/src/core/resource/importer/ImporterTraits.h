#include "utils/Base.h"

#include "core/resource/Model.h"
#include "core/resource/Image.h"
#include "core/resource/ShaderProgram.h"

#include "core/resource/importer/AssimpImporter.h"
#include "core/resource/importer/ImageImporter.h"
#include "core/resource/importer/ShaderImporter.h"

namespace Vortak {
    template <typename T>
    struct ImporterTraits;

    template <>
    struct ImporterTraits<Model> {
        using type = AssimpImporter;
    };

    template <>
    struct ImporterTraits<Image> {
        using type = ImageImporter;
    };

    template <>
    struct ImporterTraits<ShaderProgram> {
        using type = ShaderImporter;
    };
}
