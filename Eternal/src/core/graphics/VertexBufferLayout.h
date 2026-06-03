#pragma once
#include "utils/Base.h"

namespace Eternal {
    enum class ElementType : uint8_t {
        BYTE, BYTE2, BYTE3, BYTE4, UBYTE, UBYTE2, UBYTE3, UBYTE4, SHORT, SHORT2, SHORT3, SHORT4, USHORT, USHORT2,
        USHORT3, USHORT4, INT, UINT, FLOAT, FLOAT2, FLOAT3, FLOAT4, HALF, HALF2, HALF3, HALF4,
    };

    enum class VertexAttribute : uint8_t {
        POSITON, COLOR, NORMAL, UV
    };

    struct Attribute {
        uint32_t location = 0;
        VertexAttribute vertexAttribute = VertexAttribute::POSITON;
        ElementType type = ElementType::FLOAT3;
        uint32_t size = 0;
        bool normalized = false;
    };

    using AttributeData = std::vector<Attribute>;

    class VertexBufferLayout {
    public :
        VertexBufferLayout() = default;
        explicit VertexBufferLayout(const AttributeData& attributes);
        void attribute(Attribute attribute);

    private :
        AttributeData mAttributes;
    };
}
