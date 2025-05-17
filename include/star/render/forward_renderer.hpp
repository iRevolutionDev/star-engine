#pragma once

#include "star/export.hpp"
#include "star/render/renderer.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

namespace star {
    class Mesh;
    class Material;
    class Light;

    struct RenderItem {
        Entity entity{Entity::INVALID_ID};
        Mesh *mesh{nullptr};
        Material *material{nullptr};
        glm::mat4 model_matrix{1.0f};
        glm::mat4 normal_matrix{1.0f};
        float distance{0.0f};
        uint32_t sort_key{0};
    };

    class STAR_EXPORT ForwardRenderer : public Renderer {
    public:
        ForwardRenderer();

        ~ForwardRenderer() override;

        void init(Scene &scene, App &app) override;

        void shutdown() override;

        void update(float delta_time) override;

        bgfx::ViewId render_reset(bgfx::ViewId view_id) override;

        void render(bgfx::ViewId view_id, bgfx::Encoder *encoder = nullptr) override;

        RendererType get_renderer_type() const override { return RendererType::Forward; }
        std::string get_renderer_name() const override { return "ForwardRenderer"; }

        void on_window_resize(uint32_t width, uint32_t height) override;

        void set_wireframe(bool enabled);

        bool is_wireframe() const;

        void set_sort_mode(bool enabled);

        bool is_sort_mode() const;

        void set_maximum_lights(uint32_t max_lights);

        uint32_t get_maximum_lights() const;

    private:
        void collect_render_items();

        void collect_lights();

        void sort_render_items();

        void render_scene(bgfx::Encoder *encoder);

        void render_mesh(RenderItem &item, bgfx::Encoder *encoder);

        std::vector<RenderItem> _render_items;
        std::vector<Entity> _light_entities;

        bool _wireframe{false};
        bool _sort_enabled{true};
        uint32_t _max_lights{4};

        uint32_t _debug_flags{BGFX_DEBUG_NONE};
    };

    class STAR_EXPORT ForwardRendererComponent : public ITypeCameraComponent<ForwardRendererComponent> {
    public:
        ForwardRendererComponent();

        ~ForwardRendererComponent();

        void init(Camera &camera, Scene &scene, App &app) override;

        void shutdown() override;

        void render() override;

        bgfx::ViewId render_reset(bgfx::ViewId view_id) override;

        ForwardRenderer &get_renderer();

        const ForwardRenderer &get_renderer() const;

    private:
        std::unique_ptr<ForwardRenderer> _renderer;
        Camera *_camera{nullptr};
        Scene *_scene{nullptr};
        App *_app{nullptr};
        uint16_t _view_id;
    };
}
