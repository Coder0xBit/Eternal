#pragma once

#include "core/graphics/GraphicsDevice.h"
#include "core/graphics/Backend.h"
#include "utils/Base.h"

namespace Vortak {
    class IndexBuffer {
    public :
        virtual ~IndexBuffer() = default;
        virtual void bind() = 0;
        virtual void unBind() = 0;
        virtual uint32_t getCount() = 0;
        virtual void setBuffer(const std::vector<uint32_t>& indices) = 0;

        struct BuilderDetails {
            GraphicsDevice* graphicsDevice = nullptr;
            Backend backend = Backend::Vulkan;
        };

        class Builder : public utils::PrivateImplementation<BuilderDetails> {
            friend class IndexBuffer;

        public:
            Builder() noexcept;

            Builder(Builder const& rhs) noexcept;

            Builder(Builder&& rhs) noexcept;

            ~Builder() noexcept;

            Builder& operator=(Builder const& rhs) noexcept;

            Builder& operator=(Builder&& rhs) noexcept;

            Builder& graphicsDevice(GraphicsDevice* graphicsDevice) noexcept;

            Builder& backend(Backend backend) noexcept;

            std::unique_ptr<IndexBuffer> build() const noexcept;
        };

        static std::unique_ptr<IndexBuffer> create(const Builder& builder);
    };
}
