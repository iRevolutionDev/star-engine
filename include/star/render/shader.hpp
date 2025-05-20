#pragma once

#include "star/export.hpp"
#include <bgfx/embedded_shader.h>

namespace star {
    struct TextureSampler;

    struct ShaderUniform {
        bgfx::UniformHandle handle{BGFX_INVALID_HANDLE};
        std::string name;
        bgfx::UniformType::Enum type{bgfx::UniformType::Count};
        uint16_t num{1};

        ShaderUniform() = default;

        ShaderUniform(const std::string &uniform_name, bgfx::UniformType::Enum uniform_type, uint16_t count = 1);

        ~ShaderUniform();

        ShaderUniform(const ShaderUniform &) = delete;

        ShaderUniform &operator=(const ShaderUniform &) = delete;

        ShaderUniform(ShaderUniform &&other) noexcept;

        ShaderUniform &operator=(ShaderUniform &&other) noexcept;

        bool is_valid() const;

        void destroy();
    };

    class STAR_EXPORT Shader {
    public:
        Shader();

        ~Shader();

        Shader(const Shader &) = delete;

        Shader &operator=(const Shader &) = delete;

        Shader(Shader &&other) noexcept;

        Shader &operator=(Shader &&other) noexcept;

        bool load(const bgfx::Memory *vs_data, const bgfx::Memory *fs_data);

        bool load(const bgfx::EmbeddedShader &vs, const bgfx::EmbeddedShader &fs);

        bool is_valid() const;

        bgfx::ProgramHandle get_handle() const;

        ShaderUniform *get_uniform(const std::string &name);

        TextureSampler *get_sampler(const std::string &name);

        std::unordered_map<std::string, ShaderUniform> _uniforms;
        std::unordered_map<std::string, TextureSampler> _samplers;

    private:
        void destroy();

        void init_uniforms();

        bgfx::ProgramHandle _program{BGFX_INVALID_HANDLE};
    };
}
