#include "core/graphics/VertexBufferLayout.h"

namespace Vortak {
    VertexBufferLayout::VertexBufferLayout(const AttributeData& attributes) {
        mAttributes = attributes;
    }

    void VertexBufferLayout::attribute(Attribute attribute) {
        mAttributes.push_back(attribute);
    }
}
