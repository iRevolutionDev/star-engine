#pragma once

#include "star/export.hpp"
#include "star/scene/entity.hpp"
#include <glm/glm.hpp>

namespace star {
    class App;
    class Scene;
    class Transform;

    class STAR_EXPORT ICameraComponent {
    public:
        virtual ~ICameraComponent() = default;

        virtual void init(class Camera &camera, Scene &scene, App &app) {
        }

        virtual void shutdown() {
        }

        virtual void render() {
        }

        virtual bgfx::ViewId render_reset(bgfx::ViewId view_id) { return view_id; }

        virtual void before_render_view(bgfx::ViewId view_id, bgfx::Encoder &encoder) {
        }

        virtual size_t get_camera_component_type() const { return 0; }
        virtual std::string get_camera_component_name() const { return "ICameraComponent"; }
    };

    template<typename T>
    class STAR_EXPORT ITypeCameraComponent : public ICameraComponent {
    public:
        size_t get_camera_component_type() const override {
            return typeid(T).hash_code();
        }

        std::string get_camera_component_name() const override {
            return typeid(T).name();
        }
    };

    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    class STAR_EXPORT ICullingFilter {
    public:
        virtual ~ICullingFilter() = default;

        virtual bool is_visible(const glm::vec3 &position, float radius) const = 0;
    };

    class STAR_EXPORT Culling2D final : public ICullingFilter {
    public:
        bool is_visible(const glm::vec3 &position, float radius) const override;
    };

    class STAR_EXPORT Culling3D final : public ICullingFilter {
    public:
        bool is_visible(const glm::vec3 &position, float radius) const override;
    };

    class STAR_EXPORT Camera {
    public:
        Camera();

        ~Camera();

        void init(Scene &scene, App &app);

        void shutdown();

        glm::mat4 get_view_matrix() const;

        Camera &set_perspective(float fov_degrees, float near_clip, float far_clip);

        Camera &set_ortho(const glm::vec2 &size, float near_clip = -1000.0f, float far_clip = 1000.0f);

        Camera &set_ortho(float width, float height, float near_clip = -1000.0f, float far_clip = 1000.0f);

        glm::mat4 get_projection_matrix() const;

        ProjectionType get_projection_type() const;

        void set_viewport(const glm::vec4 &viewport);

        const glm::vec4 &get_viewport() const;

        void set_clear_color(const glm::vec4 &color);

        const glm::vec4 &get_clear_color() const;

        void set_clear_flags(uint16_t flags);

        uint16_t get_clear_flags() const;

        bgfx::ViewId render_reset(bgfx::ViewId view_id);

        void render();

        template<typename T, typename... Args>
        T &add_component(Args &&... args) {
            static_assert(std::is_base_of_v<ICameraComponent, T>, "T must be derived from ICameraComponent");
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T &ref = *component;
            add_component_impl(std::move(component));
            return ref;
        }

        template<typename T>
        T *get_component() {
            return static_cast<T *>(get_component_impl(typeid(T).hash_code()));
        }

        template<typename T>
        bool remove_component() {
            return remove_component_impl(typeid(T).hash_code());
        }

        template<typename T>
        Camera &set_culling_filter() {
            static_assert(std::is_base_of_v<ICullingFilter, T>, "T must be derived from ICullingFilter");
            return set_culling_filter_impl(std::make_unique<T>());
        }

        Camera &set_culling_filter(std::unique_ptr<ICullingFilter> &&filter);

        const ICullingFilter *get_culling_filter() const;

        glm::vec3 screen_to_world_point(const glm::vec2 &screen_pos, float depth = 0.0f) const;

        glm::vec2 world_to_screen_point(const glm::vec3 &world_pos) const;

        glm::vec3 screen_to_viewport_point(const glm::vec2 &screen_pos) const;

        glm::vec2 viewport_to_screen_point(const glm::vec3 &viewport_pos) const;

        struct Ray {
            glm::vec3 origin;
            glm::vec3 direction;
        };

        Ray screen_point_to_ray(const glm::vec2 &screen_pos) const;

    private:
        void add_component_impl(std::unique_ptr<ICameraComponent> &&component);

        ICameraComponent *get_component_impl(size_t type_hash);

        bool remove_component_impl(size_t type_hash);

        Camera &set_culling_filter_impl(std::unique_ptr<ICullingFilter> &&filter);

        class CameraImpl;
        std::unique_ptr<CameraImpl> _impl;
    };
}
