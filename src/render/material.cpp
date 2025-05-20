#include "star/core/common.hpp"
#include "star/render/material.hpp"
#include "star/graphics/shaders.hpp"

#include <ranges>

namespace star {
    ShaderUniform::ShaderUniform(const std::string &uniform_name, bgfx::UniformType::Enum uniform_type, uint16_t count)
        : name(uniform_name)
          , type(uniform_type)
          , num(count) {
        handle = bgfx::createUniform(name.c_str(), type, num);
    }

    ShaderUniform::~ShaderUniform() {
        destroy();
    }

    ShaderUniform::ShaderUniform(ShaderUniform &&other) noexcept
        : handle(other.handle)
          , name(std::move(other.name))
          , type(other.type)
          , num(other.num) {
        other.handle = BGFX_INVALID_HANDLE;
    }

    ShaderUniform &ShaderUniform::operator=(ShaderUniform &&other) noexcept {
        if (this != &other) {
            destroy();

            handle = other.handle;
            name = std::move(other.name);
            type = other.type;
            num = other.num;

            other.handle = BGFX_INVALID_HANDLE;
        }
        return *this;
    }

    bool ShaderUniform::is_valid() const {
        return bgfx::isValid(handle);
    }

    void ShaderUniform::destroy() {
        if (is_valid()) {
            bgfx::destroy(handle);
            handle = BGFX_INVALID_HANDLE;
        }
    }

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

    Shader::Shader() = default;

    Shader::~Shader() {
        destroy();
    }

    Shader::Shader(Shader &&other) noexcept
        : _uniforms(std::move(other._uniforms))
          , _samplers(std::move(other._samplers))
          , _program(other._program) {
        other._program = BGFX_INVALID_HANDLE;
    }

    Shader &Shader::operator=(Shader &&other) noexcept {
        if (this != &other) {
            destroy();

            _program = other._program;
            _uniforms = std::move(other._uniforms);
            _samplers = std::move(other._samplers);

            other._program = BGFX_INVALID_HANDLE;
        }
        return *this;
    }

    bool Shader::load(const bgfx::Memory *vs_data, const bgfx::Memory *fs_data) {
        destroy();

        if (!vs_data || !fs_data) {
            spdlog::error("Shader::load - Invalid shader data");
            return false;
        }

        const bgfx::ShaderHandle vsh = bgfx::createShader(vs_data);
        const bgfx::ShaderHandle fsh = bgfx::createShader(fs_data);

        if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
            spdlog::error("Shader::load - Failed to create shader handles");
            return false;
        }

        _program = bgfx::createProgram(vsh, fsh, true);

        if (!is_valid()) {
            spdlog::error("Shader::load - Failed to create shader program");
            return false;
        }

        init_uniforms();

        return true;
    }

    bool Shader::load(const bgfx::EmbeddedShader &vs, const bgfx::EmbeddedShader &fs) {
        destroy();

        const bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(&vs, bgfx::getRendererType(), vs.name);
        const bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(&fs, bgfx::getRendererType(), fs.name);

        if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
            spdlog::error("Shader::load - Failed to create embedded shader handles");
            return false;
        }

        _program = bgfx::createProgram(vsh, fsh, true);

        if (!is_valid()) {
            spdlog::error("Shader::load - Failed to create embedded shader program");
            return false;
        }

        init_uniforms();

        return true;
    }

    bool Shader::is_valid() const {
        return bgfx::isValid(_program);
    }

    bgfx::ProgramHandle Shader::get_handle() const {
        return _program;
    }

    ShaderUniform *Shader::get_uniform(const std::string &name) {
        auto it = _uniforms.find(name);
        if (it != _uniforms.end()) {
            return &it->second;
        }
        return nullptr;
    }

    TextureSampler *Shader::get_sampler(const std::string &name) {
        auto it = _samplers.find(name);
        if (it != _samplers.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void Shader::destroy() {
        _uniforms.clear();
        _samplers.clear();

        if (is_valid()) {
            bgfx::destroy(_program);
            _program = BGFX_INVALID_HANDLE;
        }
    }

    void Shader::init_uniforms() {
        _uniforms.emplace("u_color", ShaderUniform("u_color", bgfx::UniformType::Vec4));
        _uniforms.emplace("u_baseColor", ShaderUniform("u_baseColor", bgfx::UniformType::Vec4));
        _uniforms.emplace("u_emissive", ShaderUniform("u_emissive", bgfx::UniformType::Vec4));
        _uniforms.emplace("u_materialParams", ShaderUniform("u_materialParams", bgfx::UniformType::Vec4));

        _samplers.emplace("s_texColor", TextureSampler("s_texColor", 0));
        _samplers.emplace("s_texNormal", TextureSampler("s_texNormal", 1));
        _samplers.emplace("s_texMetallicRoughness", TextureSampler("s_texMetallicRoughness", 2));
        _samplers.emplace("s_texEmissive", TextureSampler("s_texEmissive", 3));
    }

    Material::Material() {
        update_state();
    }

    Material::~Material() = default;

    Material::Material(Material &&other) noexcept
        : _shader(std::move(other._shader))
          , _state(other._state)
          , _depth_test(other._depth_test)
          , _depth_write(other._depth_write)
          , _depth_func(other._depth_func)
          , _blend_mode(other._blend_mode)
          , _cull_mode(other._cull_mode) {
    }

    Material &Material::operator=(Material &&other) noexcept {
        if (this != &other) {
            _shader = std::move(other._shader);
            _state = other._state;
            _depth_test = other._depth_test;
            _depth_write = other._depth_write;
            _depth_func = other._depth_func;
            _blend_mode = other._blend_mode;
            _cull_mode = other._cull_mode;
        }
        return *this;
    }

    bool Material::set_shader(Shader &&shader) {
        if (!shader.is_valid()) {
            return false;
        }

        _shader = std::move(shader);
        return true;
    }

    bool Material::set_texture(const std::string &sampler_name, bgfx::TextureHandle texture, uint32_t flags) {
        TextureSampler *sampler = _shader.get_sampler(sampler_name);
        if (!sampler) {
            return false;
        }

        sampler->handle = texture;
        sampler->flags = flags;
        return true;
    }

    bool Material::set_uniform(const std::string &name, const glm::vec4 &value) {
        const ShaderUniform *uniform = _shader.get_uniform(name);
        if (!uniform || uniform->type != bgfx::UniformType::Vec4) {
            return false;
        }

        bgfx::setUniform(uniform->handle, &value);
        return true;
    }

    bool Material::set_uniform(const std::string &name, const glm::mat4 &value) {
        ShaderUniform *uniform = _shader.get_uniform(name);
        if (!uniform || uniform->type != bgfx::UniformType::Mat4) {
            return false;
        }

        bgfx::setUniform(uniform->handle, &value);
        return true;
    }

    bool Material::set_uniform(const std::string &name, const float *data, uint16_t count) {
        ShaderUniform *uniform = _shader.get_uniform(name);
        if (!uniform) {
            return false;
        }

        bgfx::setUniform(uniform->handle, data, count);
        return true;
    }

    void Material::set_blend_mode(BlendMode mode) {
        _blend_mode = mode;
        update_state();
    }

    BlendMode Material::get_blend_mode() const {
        return _blend_mode;
    }

    void Material::set_depth_test(bool enabled) {
        _depth_test = enabled;
        update_state();
    }

    bool Material::get_depth_test() const {
        return _depth_test;
    }

    void Material::set_depth_write(bool enabled) {
        _depth_write = enabled;
        update_state();
    }

    bool Material::get_depth_write() const {
        return _depth_write;
    }

    void Material::set_depth_function(DepthFunc func) {
        _depth_func = func;
        update_state();
    }

    DepthFunc Material::get_depth_function() const {
        return _depth_func;
    }

    void Material::set_cull_mode(CullMode mode) {
        _cull_mode = mode;
        update_state();
    }

    CullMode Material::get_cull_mode() const {
        return _cull_mode;
    }

    void Material::bind(bgfx::Encoder *encoder, uint8_t view_id) const {
        if (!_shader.is_valid()) {
            spdlog::warn("Material::bind - Invalid shader");
            return;
        }

        encoder->setState(_state);

        for (const auto &sampler: _shader._samplers | std::views::values) {
            if (bgfx::isValid(sampler.handle)) {
                encoder->setTexture(sampler.stage, sampler.sampler, sampler.handle, sampler.flags);
            }
        }

        encoder->submit(view_id, _shader.get_handle());
    }

    uint32_t Material::generate_sort_key() const {
        const uint32_t type_key = static_cast<uint32_t>(get_type()) << 24;
        const uint32_t blend_key = static_cast<uint32_t>(_blend_mode) << 20;
        const uint32_t shader_key = _shader.is_valid() ? static_cast<uint32_t>(_shader.get_handle().idx & 0xFFFFF) : 0;

        return type_key | blend_key | shader_key;
    }

    void Material::update_state() {
        _state = BGFX_STATE_WRITE_RGB | BGFX_STATE_MSAA;

        if (_depth_test) {
            switch (_depth_func) {
                case DepthFunc::Less:
                    _state |= BGFX_STATE_DEPTH_TEST_LESS;
                    break;
                case DepthFunc::LessEqual:
                    _state |= BGFX_STATE_DEPTH_TEST_LEQUAL;
                    break;
                case DepthFunc::Equal:
                    _state |= BGFX_STATE_DEPTH_TEST_EQUAL;
                    break;
                case DepthFunc::GreaterEqual:
                    _state |= BGFX_STATE_DEPTH_TEST_GEQUAL;
                    break;
                case DepthFunc::Greater:
                    _state |= BGFX_STATE_DEPTH_TEST_GREATER;
                    break;
                case DepthFunc::NotEqual:
                    _state |= BGFX_STATE_DEPTH_TEST_NOTEQUAL;
                    break;
                case DepthFunc::Always:
                    _state |= BGFX_STATE_DEPTH_TEST_ALWAYS;
                    break;
                case DepthFunc::Never:
                    _state |= BGFX_STATE_DEPTH_TEST_NEVER;
                    break;
            }
        }

        if (_depth_write) {
            _state |= BGFX_STATE_WRITE_Z;
        }

        switch (_cull_mode) {
            case CullMode::None:
                break;
            case CullMode::CW:
                _state |= BGFX_STATE_CULL_CW;
                break;
            case CullMode::CCW:
                _state |= BGFX_STATE_CULL_CCW;
                break;
        }

        switch (_blend_mode) {
            case BlendMode::Opaque:
                break;
            case BlendMode::Alpha:
                _state |= BGFX_STATE_BLEND_ALPHA;
                break;
            case BlendMode::Additive:
                _state |= BGFX_STATE_BLEND_ADD;
                break;
            case BlendMode::Multiply:
                _state |= BGFX_STATE_BLEND_MULTIPLY;
                break;
        }
    }

    UnlitMaterial::UnlitMaterial() {
        _shader.load(k_simple_vs, k_simple_fs);
    }

    UnlitMaterial::~UnlitMaterial() = default;

    void UnlitMaterial::set_color(const glm::vec4 &color) {
        _color = color;
        set_uniform("u_color", _color);
    }

    glm::vec4 UnlitMaterial::get_color() const {
        return _color;
    }

    StandardMaterial::StandardMaterial() {
    }

    StandardMaterial::~StandardMaterial() = default;

    void StandardMaterial::set_base_color(const glm::vec4 &color) {
        _base_color = color;
        set_uniform("u_baseColor", _base_color);
    }

    glm::vec4 StandardMaterial::get_base_color() const {
        return _base_color;
    }

    void StandardMaterial::set_metallic(float value) {
        _metallic = glm::clamp(value, 0.0f, 1.0f);
        glm::vec4 params(_metallic, _roughness, 0.0f, 0.0f);
        set_uniform("u_materialParams", params);
    }

    float StandardMaterial::get_metallic() const {
        return _metallic;
    }

    void StandardMaterial::set_roughness(float value) {
        _roughness = glm::clamp(value, 0.0f, 1.0f);
        glm::vec4 params(_metallic, _roughness, 0.0f, 0.0f);
        set_uniform("u_materialParams", params);
    }

    float StandardMaterial::get_roughness() const {
        return _roughness;
    }

    void StandardMaterial::set_emissive(const glm::vec3 &value) {
        _emissive = value;
        set_uniform("u_emissive", glm::vec4(_emissive, 1.0f));
    }

    glm::vec3 StandardMaterial::get_emissive() const {
        return _emissive;
    }
}
