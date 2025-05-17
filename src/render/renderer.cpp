#include "star/render/renderer.hpp"
#include "star/scene/scene.hpp"
#include "star/app/app.hpp"
#include <spdlog/spdlog.h>

namespace star {
    Renderer::Renderer() = default;

    Renderer::~Renderer() = default;

    void Renderer::init(Scene &scene, App &app) {
        _scene = &scene;
        _app = &app;

        spdlog::debug("Initialized renderer: {}", get_renderer_name());
    }

    void Renderer::shutdown() {
        spdlog::debug("Shutting down renderer: {}", get_renderer_name());

        _scene = nullptr;
        _app = nullptr;
    }

    void Renderer::update(float delta_time) {
    }

    bgfx::ViewId Renderer::render_reset(bgfx::ViewId view_id) {
        if (!_visible) {
            return view_id;
        }

        _view_id = view_id;
        return view_id + 1;
    }

    void Renderer::render(bgfx::ViewId view_id, bgfx::Encoder *encoder) {
        if (!_visible || !_scene) {
            return;
        }
    }

    void Renderer::set_visible(bool visible) {
        _visible = visible;
    }

    bool Renderer::is_visible() const {
        return _visible;
    }

    void Renderer::set_debug_enabled(bool enabled) {
        _debug_enabled = enabled;
    }

    bool Renderer::is_debug_enabled() const {
        return _debug_enabled;
    }

    void Renderer::set_camera(Entity camera_entity) {
        _camera_entity = camera_entity;
    }

    Entity Renderer::get_camera() const {
        return _camera_entity;
    }
}
