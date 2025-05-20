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

    void ForwardRenderer::update(const float delta_time) {
        Renderer::update(delta_time);

        _render_items.clear();
        _light_entities.clear();

        if (_visible && _scene) {
            collect_render_items();
            collect_lights();

            if (_sort_enabled) {
                sort_render_items();
            }
        }
    }

    bgfx::ViewId ForwardRenderer::render_reset(const bgfx::ViewId view_id) {
        if (!_visible) {
            return view_id;
        }

        _view_id = view_id;

        bgfx::setDebug(_debug_flags);

        return view_id + 1;
    }

    void ForwardRenderer::render(const bgfx::ViewId view_id, bgfx::Encoder *encoder) {
        if (!_visible || !_scene) {
            return;
        }

        bgfx::ViewId currentViewId = (view_id != UINT16_MAX) ? view_id : _view_id;

        bgfx::Encoder *enc = encoder;
        bool created_encoder = false;
        if (enc == nullptr) {
            enc = bgfx::begin();
            created_encoder = true;
        }

        render_scene(enc);

        if (created_encoder) {
            bgfx::end(enc);
        }
    }

    void ForwardRenderer::on_window_resize(uint32_t width, uint32_t height) {
    }

    void ForwardRenderer::set_wireframe(const bool enabled) {
        _wireframe = enabled;

        if (_wireframe) {
            _debug_flags |= BGFX_DEBUG_WIREFRAME;
        } else {
            _debug_flags &= ~BGFX_DEBUG_WIREFRAME;
        }
    }

    bool ForwardRenderer::is_wireframe() const {
        return _wireframe;
    }

    void ForwardRenderer::set_sort_mode(bool enabled) {
        _sort_enabled = enabled;
    }

    bool ForwardRenderer::is_sort_mode() const {
        return _sort_enabled;
    }

    void ForwardRenderer::set_maximum_lights(uint32_t max_lights) {
        _max_lights = max_lights;
    }

    uint32_t ForwardRenderer::get_maximum_lights() const {
        return _max_lights;
    }

    void ForwardRenderer::collect_render_items() {
        if (!_scene) return;

        auto view = _scene->get_registry().view<MeshRenderer, Transform>();

        for (auto entity: view) {
            const auto &mesh_renderer = view.get<MeshRenderer>(entity);
            const auto &transform = view.get<Transform>(entity);

            if (!mesh_renderer.is_visible()) continue;

            auto *mesh = mesh_renderer.get_mesh();
            auto *material = mesh_renderer.get_material();

            if (!mesh || !mesh->is_valid() || !material) continue;

            RenderItem item;
            item.entity = entity;
            item.mesh = mesh;
            item.material = material;
            item.model_matrix = transform.get_model_matrix();
            item.normal_matrix = glm::transpose(glm::inverse(glm::mat3(item.model_matrix)));
            item.sort_key = mesh_renderer.generate_sort_key();
            if (_camera_entity != entt::null && _scene->get_registry().valid(_camera_entity)) {
                if (const auto *camera_transform = _scene->get_registry().try_get<Transform>(_camera_entity)) {
                    const glm::vec3 camera_pos = camera_transform->get_position();
                    const glm::vec3 object_pos = transform.get_position();
                    item.distance = glm::distance(camera_pos, object_pos);
                }
            }

            _render_items.push_back(item);
        }
    }

    void ForwardRenderer::collect_lights() {
        if (!_scene) return;

        auto view = _scene->get_registry().view<Light, Transform>();

        for (auto entity: view) {
            const auto &light = view.get<Light>(entity);

            if (!light.is_enabled()) continue;

            _light_entities.push_back(Entity(entity));

            if (_light_entities.size() >= _max_lights) {
                break;
            }
        }
    }

    void ForwardRenderer::sort_render_items() {
        if (_render_items.empty()) return;

        std::ranges::sort(_render_items,
                          [](const RenderItem &a, const RenderItem &b) {
                              if (a.sort_key != b.sort_key) {
                                  return a.sort_key < b.sort_key;
                              }

                              return a.distance < b.distance;
                          });
    }

    void ForwardRenderer::render_scene(bgfx::Encoder *encoder) {
        for (auto &item: _render_items) {
            render_mesh(item, encoder);
        }
    }

    void ForwardRenderer::render_mesh(RenderItem &item, bgfx::Encoder *encoder) {
        if (!item.mesh || !item.material) return;
        bgfx::setTransform(&item.model_matrix[0][0]);
        item.material->bind(encoder, _view_id);
        item.mesh->draw(encoder);
    }

    ForwardRendererComponent::ForwardRendererComponent()
        : _renderer(std::make_unique<ForwardRenderer>()), _view_id(0) {
    }

    ForwardRendererComponent::~ForwardRendererComponent() = default;

    void ForwardRendererComponent::init(Camera &camera, Scene &scene, App &app) {
        _camera = &camera;
        _scene = &scene;
        _app = &app;

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
        if (!_renderer || !_camera) return;
        _renderer->set_camera(_camera->get_entity());

        _renderer->render(_view_id);
    }

    bgfx::ViewId ForwardRendererComponent::render_reset(bgfx::ViewId view_id) {
        if (!_renderer) return view_id;

        _view_id = view_id;

        return _renderer->render_reset(view_id);
    }

    ForwardRenderer &ForwardRendererComponent::get_renderer() {
        return *_renderer;
    }

    const ForwardRenderer &ForwardRendererComponent::get_renderer() const {
        return *_renderer;
    }
}
