#pragma once

namespace star {
    struct TextureSampler {
        bgfx::TextureHandle handle{BGFX_INVALID_HANDLE};
        bgfx::UniformHandle sampler{BGFX_INVALID_HANDLE};
        std::string name;
        uint8_t stage{0};
        uint32_t flags{BGFX_SAMPLER_NONE};

        TextureSampler() = default;

        TextureSampler(const std::string &sampler_name, uint8_t texture_stage = 0);

        ~TextureSampler();

        TextureSampler(const TextureSampler &) = delete;

        TextureSampler &operator=(const TextureSampler &) = delete;

        TextureSampler(TextureSampler &&other) noexcept;

        TextureSampler &operator=(TextureSampler &&other) noexcept;

        bool is_valid() const;

        void destroy();
    };
}
