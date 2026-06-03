#include "core/graphics/VertexBufferLayout.h"

namespace Eternal {
    VertexBufferLayout::VertexBufferLayout(const AttributeData& attributes) {
        mAttributes = attributes;
    }

    void VertexBufferLayout::attribute(Attribute attribute) {
        mAttributes.push_back(attribute);
    }
}
