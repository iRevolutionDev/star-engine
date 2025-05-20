#include "star/render/forward_renderer.hpp"
#include "star/scene/scene.hpp"
#include "star/scene/camera.hpp"
#include "star/app/app.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

#include "star/render/material.hpp"
#include "star/render/renderer_components.hpp"
#include "star/scene/transform.hpp"

namespace star {
    ForwardRenderer::ForwardRenderer() = default;

    ForwardRenderer::~ForwardRenderer() = default;

    void ForwardRenderer::init(Scene &scene, App &app) {
        Renderer::init(scene, app);
        spdlog::debug("Forward renderer initialized");
    }

    void ForwardRenderer::shutdown() {
        Renderer::shutdown();
        spdlog::debug("Forward renderer shut down");
    }

    bgfx::ViewId ForwardRenderer::render_reset(bgfx::ViewId view_id) {
        _view_id.reset();
        if (!_camera) {
            return view_id;
        }

        _camera->configure_view(view_id, "Forward");
        _view_id = view_id;
        return ++view_id;
    }

    void ForwardRenderer::render(const bgfx::ViewId view_id, bgfx::Encoder *encoder) {
        auto entities = _scene->get_registry().view<MeshRenderer>();
        for (auto entity: entities) {
            auto &mesh_renderer = entities.get<MeshRenderer>(entity);

            if (!mesh_renderer.is_visible()) {
                continue;
            }

            if (!mesh_renderer.render(encoder)) {
                continue;
            }
        }
    }

    ForwardRendererComponent::ForwardRendererComponent()
        : _renderer(std::make_unique<ForwardRenderer>()), _view_id(0) {
    }

    ForwardRendererComponent::~ForwardRendererComponent() = default;

    void ForwardRendererComponent::init(Camera &camera, Scene &scene, App &app) {
        _camera = camera;
        _scene = scene;
        _app = app;

        _renderer->init(scene, app);
    }

    void ForwardRendererComponent::shutdown() {
        if (_renderer) {
            _renderer->shutdown();
        }

        _camera = nullptr;
        _scene = nullptr;
        _app = nullptr;
    }

    void ForwardRendererComponent::render() {
        if (!_scene || !_camera || !_camera->is_valid() || !_camera->is_enabled()) {
            return;
        }

        const auto view_id = _view_id.value();
        auto &encoder = *bgfx::begin();

        _renderer->render(view_id, &encoder);

        bgfx::end(&encoder);
    }

    bgfx::ViewId ForwardRendererComponent::render_reset(const bgfx::ViewId view_id) {
        return _renderer->render_reset(view_id);
    }

    ForwardRenderer &ForwardRendererComponent::get_renderer() {
        return *_renderer;
    }

    const ForwardRenderer &ForwardRendererComponent::get_renderer() const {
        return *_renderer;
    }
}
