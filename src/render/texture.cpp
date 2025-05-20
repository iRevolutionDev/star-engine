#include "star/core/common.hpp"
#include "star/render/texture.hpp"

namespace star {
    TextureSampler::TextureSampler(const std::string &sampler_name, uint8_t texture_stage)
        : name(sampler_name)
          , stage(texture_stage) {
        sampler = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler, 1);
    }

    TextureSampler::~TextureSampler() {
        destroy();
    }

    TextureSampler::TextureSampler(TextureSampler &&other) noexcept
        : handle(other.handle)
          , sampler(other.sampler)
          , name(std::move(other.name))
          , stage(other.stage)
          , flags(other.flags) {
        other.handle = BGFX_INVALID_HANDLE;
        other.sampler = BGFX_INVALID_HANDLE;
    }

    TextureSampler &TextureSampler::operator=(TextureSampler &&other) noexcept {
        if (this != &other) {
            destroy();

            handle = other.handle;
            sampler = other.sampler;
            name = std::move(other.name);
            stage = other.stage;
            flags = other.flags;

            other.handle = BGFX_INVALID_HANDLE;
            other.sampler = BGFX_INVALID_HANDLE;
        }
        return *this;
    }

    bool TextureSampler::is_valid() const {
        return bgfx::isValid(sampler);
    }

    void TextureSampler::destroy() {
        if (is_valid()) {
            bgfx::destroy(sampler);
            sampler = BGFX_INVALID_HANDLE;
        }

        handle = BGFX_INVALID_HANDLE;
    }
}
