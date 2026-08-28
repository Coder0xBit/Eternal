#pragma once
#include "core/graphics/Shader.h"
#include "core/resource/ShaderProgram.h"
#include "core/graphics/VertexBufferLayout.h"

namespace Vortak {
    enum class PrimitiveTopology : uint8_t {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan
    };

    enum class CullMode : uint8_t {
        None,
        Front,
        Back
    };

    enum class FrontFace : uint8_t {
        Clockwise,
        CounterClockwise
    };

    enum class PolygonMode : uint8_t {
        Fill,
        Line,
        Point
    };

    enum class CompareOperation : uint8_t {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    struct RasterizerState {
        CullMode cullMode = CullMode::Back;
        FrontFace frontFace = FrontFace::CounterClockwise;
        PolygonMode polygonMode = PolygonMode::Fill;
    };

    struct DepthStencilState {
        bool depthTest = true;
        bool depthWrite = true;

        CompareOperation depthCompare =
            CompareOperation::Less;
    };

    struct BlendState {
        bool enabled = false;
    };

    struct PipelineDescription {
        Shader* vertexShader = nullptr;
        Shader* fragmentShader = nullptr;

        VertexBufferLayout vertexLayout;

        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

        RasterizerState rasterizerState;

        DepthStencilState depthStencilState;

        BlendState blendState;
    };
}
