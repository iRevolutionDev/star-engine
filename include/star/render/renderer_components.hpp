#pragma once

#include "star/export.hpp"
#include "star/render/mesh.hpp"
#include "star/render/material.hpp"
#include <memory>

namespace star {
    class STAR_EXPORT MeshRenderer {
    public:
        MeshRenderer();

        ~MeshRenderer();

        void set_mesh(Mesh &&mesh);

        void set_mesh(std::shared_ptr<Mesh> mesh);

        Mesh *get_mesh() const;

        void set_material(std::shared_ptr<Material> material);

        Material *get_material() const;

        void set_visible(bool visible);

        bool is_visible() const;

        void set_layer(uint8_t layer);

        uint8_t get_layer() const;

        uint32_t generate_sort_key() const;

    private:
        std::shared_ptr<Mesh> _mesh;
        std::shared_ptr<Material> _material;
        bool _visible{true};
        uint8_t _layer{0};
    };

    enum class LightType {
        Directional,
        Point,
        Spot
    };

    class STAR_EXPORT Light {
    public:
        Light();

        ~Light();

        void set_type(LightType type);

        LightType get_type() const;

        void set_color(const glm::vec3 &color);

        const glm::vec3 &get_color() const;

        void set_intensity(float intensity);

        float get_intensity() const;

        void set_range(float range);

        float get_range() const;

        void set_inner_angle(float angle_degrees);

        float get_inner_angle() const;

        void set_outer_angle(float angle_degrees);

        float get_outer_angle() const;

        void set_enabled(bool enabled);

        bool is_enabled() const;

        void set_cast_shadows(bool cast);

        bool get_cast_shadows() const;

        void get_light_data(glm::vec4 &position_range, glm::vec4 &color_intensity,
                            glm::vec4 &direction_spot) const;

    private:
        LightType _type{LightType::Directional};
        glm::vec3 _color{1.0f};
        float _intensity{1.0f};
        float _range{10.0f};
        float _inner_angle{15.0f};
        float _outer_angle{30.0f};
        bool _enabled{true};
        bool _cast_shadows{false};
    };
}
