#include "star/render/scene_renderer.hpp"
#include "star/scene/scene.hpp"
#include "star/app/app.hpp"
#include <spdlog/spdlog.h>

namespace star {
    SceneRendererComponent::SceneRendererComponent()
        : _renderer(std::make_unique<ForwardRenderer>()) {
    }

    SceneRendererComponent::~SceneRendererComponent() = default;

    void SceneRendererComponent::init(Scene &scene, App &app) {
        _scene = &scene;
        _app = &app;

        _renderer->init(scene, app);

        spdlog::debug("SceneRendererComponent initialized");
    }

    void SceneRendererComponent::shutdown() {
        if (_renderer) {
            _renderer->shutdown();
        }

        _scene = nullptr;
        _app = nullptr;

        spdlog::debug("SceneRendererComponent shut down");
    }

    void SceneRendererComponent::update(float delta_time) {
        if (_renderer) {
            _renderer->update(delta_time);
        }
    }

    bgfx::ViewId SceneRendererComponent::render_reset(bgfx::ViewId view_id) {
        if (!_renderer) return view_id;

        return _renderer->render_reset(view_id);
    }

    ForwardRenderer &SceneRendererComponent::get_renderer() {
        return *_renderer;
    }

    const ForwardRenderer &SceneRendererComponent::get_renderer() const {
        return *_renderer;
    }
}
