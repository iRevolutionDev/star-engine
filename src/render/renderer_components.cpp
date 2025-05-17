#include "star/render/renderer_components.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace star {
    MeshRenderer::MeshRenderer() = default;

    MeshRenderer::~MeshRenderer() = default;

    void MeshRenderer::set_mesh(Mesh &&mesh) {
        _mesh = std::make_shared<Mesh>(std::move(mesh));
    }

    void MeshRenderer::set_mesh(std::shared_ptr<Mesh> mesh) {
        _mesh = mesh;
    }

    Mesh *MeshRenderer::get_mesh() const {
        return _mesh.get();
    }

    void MeshRenderer::set_material(std::shared_ptr<Material> material) {
        _material = material;
    }

    Material *MeshRenderer::get_material() const {
        return _material.get();
    }

    void MeshRenderer::set_visible(bool visible) {
        _visible = visible;
    }

    bool MeshRenderer::is_visible() const {
        return _visible;
    }

    void MeshRenderer::set_layer(uint8_t layer) {
        _layer = layer;
    }

    uint8_t MeshRenderer::get_layer() const {
        return _layer;
    }

    uint32_t MeshRenderer::generate_sort_key() const {
        uint32_t material_key = 0;

        if (_material) {
            material_key = _material->generate_sort_key();
        }

        uint32_t layer_key = static_cast<uint32_t>(_layer) << 28;

        return layer_key | material_key;
    }

    Light::Light() = default;

    Light::~Light() = default;

    void Light::set_type(LightType type) {
        _type = type;
    }

    LightType Light::get_type() const {
        return _type;
    }

    void Light::set_color(const glm::vec3 &color) {
        _color = color;
    }

    const glm::vec3 &Light::get_color() const {
        return _color;
    }

    void Light::set_intensity(float intensity) {
        _intensity = glm::max(0.0f, intensity);
    }

    float Light::get_intensity() const {
        return _intensity;
    }

    void Light::set_range(float range) {
        _range = glm::max(0.0f, range);
    }

    float Light::get_range() const {
        return _range;
    }

    void Light::set_inner_angle(float angle_degrees) {
        _inner_angle = glm::clamp(angle_degrees, 0.0f, _outer_angle);
    }

    float Light::get_inner_angle() const {
        return _inner_angle;
    }

    void Light::set_outer_angle(float angle_degrees) {
        _outer_angle = glm::clamp(angle_degrees, _inner_angle, 90.0f);
    }

    float Light::get_outer_angle() const {
        return _outer_angle;
    }

    void Light::set_enabled(bool enabled) {
        _enabled = enabled;
    }

    bool Light::is_enabled() const {
        return _enabled;
    }

    void Light::set_cast_shadows(bool cast) {
        _cast_shadows = cast;
    }

    bool Light::get_cast_shadows() const {
        return _cast_shadows;
    }

    void Light::get_light_data(glm::vec4 &position_range, glm::vec4 &color_intensity,
                               glm::vec4 &direction_spot) const {
        position_range.w = (_type == LightType::Directional) ? 0.0f : _range;

        color_intensity = glm::vec4(_color, _intensity);

        if (_type == LightType::Spot) {
            float inner_cos = glm::cos(glm::radians(_inner_angle));
            float outer_cos = glm::cos(glm::radians(_outer_angle));

            direction_spot.w = inner_cos - outer_cos;
        } else {
            direction_spot.w = 0.0f;
        }
    }
}
