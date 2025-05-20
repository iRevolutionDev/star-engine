#include "star/app/app.hpp"
#include "star/app/window.hpp"
#include "star/scene/scene.hpp"
#include "star/scene/camera.hpp"
#include "star/scene/transform.hpp"
#include "star/render/forward_renderer.hpp"
#include "star/render/scene_renderer.hpp"
#include "star/render/renderer_components.hpp"
#include "star/render/mesh.hpp"
#include "star/render/material.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

using namespace star;

class BasicSampleApp final : public IAppDelegate {
public:
    explicit BasicSampleApp(App &app) : _app(app), _cube_entity() {
    }

    void init() override {
        _app.get_window().set_title("Star Engine Basic Sample");

        _app.set_debug_flag(BGFX_DEBUG_TEXT);

        _scene_component = &_app.add_component<SceneAppComponent>();
        _scene = _scene_component->get_scene();
        _scene->set_name("BasicSample");

        const auto camera_entity = _scene->create_entity();

        auto &camera_transform = _scene->add_component<Transform>(camera_entity);
        camera_transform.set_position(glm::vec3(0.0f, 2.0f, -5.0f));
        camera_transform.look_at(glm::vec3(0.0f, 0.0f, 0.0f));

        auto &camera = _scene->add_component<Camera>(camera_entity);
        camera.set_perspective(60.0f, 0.1f, 1000.0f);

        camera.add_component<ForwardRendererComponent>();

        auto &renderer_component = _scene->add_scene_component<SceneRendererComponent>();

        _cube_entity = _scene->create_entity();
        auto &cube_transform = _scene->add_component<Transform>(_cube_entity);
        cube_transform.set_position(glm::vec3(0.0f, 0.0f, 0.0f));

        auto &mesh_renderer = _scene->add_component<MeshRenderer>(_cube_entity);

        Vertex::init();

        Mesh cube_mesh = Mesh::create_cube(1.0f);
        mesh_renderer.set_mesh(std::move(cube_mesh));

        const auto material = std::make_shared<UnlitMaterial>();
        material->set_color(glm::vec4(0.2f, 0.5f, 1.0f, 1.0f));
        mesh_renderer.set_material(material);

        const auto light_entity = _scene->create_entity();
        auto &light_transform = _scene->add_component<Transform>(light_entity);
        light_transform.set_position(glm::vec3(5.0f, 5.0f, -5.0f));

        auto &light = _scene->add_component<Light>(light_entity);
        light.set_type(LightType::Directional);
        light.set_color(glm::vec3(1.0f, 1.0f, 0.9f));
        light.set_intensity(1.0f);

        spdlog::info("Basic sample initialized");
    }

    void update(const float delta_time) override {
        if (_scene->is_valid_entity(_cube_entity)) {
            if (auto *transform = _scene->get_component<Transform>(_cube_entity)) {
                glm::vec3 euler = transform->get_euler_angles();
                euler.y += 45.0f * delta_time;
                transform->set_euler_angles(euler);
            }
        }
    }

    void shutdown() override {
        spdlog::info("Basic sample shutting down");
    }

private:
    App &_app;
    SceneAppComponent *_scene_component = nullptr;
    Scene *_scene = nullptr;
    Entity _cube_entity;
};

STAR_RUN_APP(BasicSampleApp);
