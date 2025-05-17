#include "star/scene/scene.hpp"
#include "star/scene/entity_registry.hpp"
#include "star/app/app.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

#include "star/scene/camera.hpp"
#include "star/scene/transform.hpp"
#include "star/render/renderer_components.hpp"

namespace star {
    class Scene::SceneImpl {
    public:
        SceneImpl(Scene &scene);

        ~SceneImpl();

        void init(App &app);

        void shutdown();

        void update(float delta_time);

        bgfx::ViewId render_reset(bgfx::ViewId view_id);

        void set_paused(bool paused);

        bool is_paused() const;

        void set_name(const std::string &name);

        const std::string &get_name() const;

        void add_scene_component(std::unique_ptr<ISceneComponent> &&component);

        ISceneComponent *get_scene_component(size_t type_hash);

        bool remove_scene_component(size_t type_hash);

        Entity create_entity();

        void destroy_entity(Entity entity);

        bool is_valid_entity(Entity entity) const;

        void set_delegate(ISceneDelegate *delegate);

        ISceneDelegate *get_delegate() const;

        EntityRegistry &get_registry();

        const EntityRegistry &get_registry() const;

        void set_view_id(bgfx::ViewId view_id);

        bgfx::ViewId get_view_id() const;

        std::string to_string() const;

    private:
        Scene &_scene;
        App *_app{nullptr};
        std::string _name;
        bool _paused{false};
        ISceneDelegate *_delegate{nullptr};
        EntityRegistry _registry;
        std::vector<std::unique_ptr<ISceneComponent> > _components;
        bgfx::ViewId _view_id{0};
    };

    Scene::SceneImpl::SceneImpl(Scene &scene)
        : _scene(scene)
          , _name("Scene") {
    }

    Scene::SceneImpl::~SceneImpl() {
        shutdown();
    }

    void Scene::SceneImpl::init(App &app) {
        _app = &app;

        for (auto &component: _components) {
            component->init(_scene, app);
        }
    }

    void Scene::SceneImpl::shutdown() {
        if (!_app) {
            return;
        }

        for (auto it = _components.rbegin(); it != _components.rend(); ++it) {
            (*it)->shutdown();
        }

        _app = nullptr;
    }

    void Scene::SceneImpl::update(float delta_time) {
        if (_paused) {
            return;
        }

        for (auto &component: _components) {
            component->update(delta_time);
        }

        if (_delegate) {
            _delegate->on_scene_updated(delta_time);
        }
    }

    bgfx::ViewId Scene::SceneImpl::render_reset(bgfx::ViewId view_id) {
        _view_id = view_id;

        for (auto &component: _components) {
            _view_id = component->render_reset(_view_id);
        }

        return _view_id;
    }

    void Scene::SceneImpl::set_paused(bool paused) {
        _paused = paused;
    }

    bool Scene::SceneImpl::is_paused() const {
        return _paused;
    }

    void Scene::SceneImpl::set_name(const std::string &name) {
        _name = name;
    }

    const std::string &Scene::SceneImpl::get_name() const {
        return _name;
    }

    void Scene::SceneImpl::add_scene_component(std::unique_ptr<ISceneComponent> &&component) {
        if (auto type_hash = component->get_scene_component_type()) {
            remove_scene_component(type_hash);
        }

        if (_app) {
            component->init(_scene, *_app);
        }

        _components.push_back(std::move(component));
    }

    ISceneComponent *Scene::SceneImpl::get_scene_component(size_t type_hash) {
        for (auto &component: _components) {
            if (component->get_scene_component_type() == type_hash) {
                return component.get();
            }
        }
        return nullptr;
    }

    bool Scene::SceneImpl::remove_scene_component(size_t type_hash) {
        auto it = std::find_if(_components.begin(), _components.end(),
                               [type_hash](const auto &component) {
                                   return component->get_scene_component_type() == type_hash;
                               });

        if (it != _components.end()) {
            if (_app) {
                (*it)->shutdown();
            }
            _components.erase(it);
            return true;
        }

        return false;
    }

    Entity Scene::SceneImpl::create_entity() {
        const Entity entity = _registry.create_entity();

        if (_delegate) {
            _delegate->on_entity_created(entity);
        }

        return entity;
    }

    void Scene::SceneImpl::destroy_entity(Entity entity) {
        if (_registry.is_valid(entity)) {
            if (_delegate) {
                _delegate->on_entity_destroyed(entity);
            }

            _registry.destroy_entity(entity);
        }
    }

    bool Scene::SceneImpl::is_valid_entity(Entity entity) const {
        return _registry.is_valid(entity);
    }

    void Scene::SceneImpl::set_delegate(ISceneDelegate *delegate) {
        _delegate = delegate;
    }

    ISceneDelegate *Scene::SceneImpl::get_delegate() const {
        return _delegate;
    }

    EntityRegistry &Scene::SceneImpl::get_registry() {
        return _registry;
    }

    const EntityRegistry &Scene::SceneImpl::get_registry() const {
        return _registry;
    }

    void Scene::SceneImpl::set_view_id(bgfx::ViewId view_id) {
        _view_id = view_id;
    }

    bgfx::ViewId Scene::SceneImpl::get_view_id() const {
        return _view_id;
    }

    std::string Scene::SceneImpl::to_string() const {
        return "Scene(" + _name + ")";
    }

    Scene::Scene() : _impl(std::make_unique<SceneImpl>(*this)) {
    }

    Scene::~Scene() = default;

    void Scene::init(App &app) const {
        _impl->init(app);
    }

    void Scene::shutdown() {
        _impl->shutdown();
    }

    void Scene::update(float delta_time) {
        _impl->update(delta_time);
    }

    bgfx::ViewId Scene::render_reset(bgfx::ViewId view_id) {
        return _impl->render_reset(view_id);
    }

    void Scene::set_paused(bool paused) const {
        _impl->set_paused(paused);
    }

    bool Scene::is_paused() const {
        return _impl->is_paused();
    }

    void Scene::set_name(const std::string &name) {
        _impl->set_name(name);
    }

    const std::string &Scene::get_name() const {
        return _impl->get_name();
    }

    void Scene::add_scene_component_impl(std::unique_ptr<ISceneComponent> &&component) {
        _impl->add_scene_component(std::move(component));
    }

    ISceneComponent *Scene::get_scene_component_impl(size_t type_hash) {
        return _impl->get_scene_component(type_hash);
    }

    bool Scene::remove_scene_component_impl(size_t type_hash) {
        return _impl->remove_scene_component(type_hash);
    }

    Entity Scene::create_entity() {
        return _impl->create_entity();
    }

    void Scene::destroy_entity(const Entity entity) {
        _impl->destroy_entity(entity);
    }

    bool Scene::is_valid_entity(const Entity entity) const {
        return _impl->is_valid_entity(entity);
    }

    void Scene::set_delegate(ISceneDelegate *delegate) {
        _impl->set_delegate(delegate);
    }    ISceneDelegate *Scene::get_delegate() const {
        return _impl->get_delegate();
    }

    template<typename T, typename... Args>
    T &Scene::add_component(Entity entity, Args &&... args) {
        return _impl->get_registry().add_component<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    T *Scene::get_component(Entity entity) {
        return _impl->get_registry().get_component<T>(entity);
    }

    template<typename T>
    bool Scene::remove_component(Entity entity) const {
        return _impl->get_registry().remove_component<T>(entity);
    }

    template<typename T>
    bool Scene::has_component(Entity entity) const {
        return _impl->get_registry().has_component<T>(entity);
    }

    template Transform& Scene::add_component<Transform>(Entity);
    template Transform* Scene::get_component<Transform>(Entity);
    template bool Scene::remove_component<Transform>(Entity) const;
    template bool Scene::has_component<Transform>(Entity) const;
    template Camera& Scene::add_component<Camera>(Entity);
    template Camera* Scene::get_component<Camera>(Entity);
    template bool Scene::remove_component<Camera>(Entity) const;
    template bool Scene::has_component<Camera>(Entity) const;
    template MeshRenderer& Scene::add_component<MeshRenderer>(Entity);
    template MeshRenderer* Scene::get_component<MeshRenderer>(Entity);
    template bool Scene::remove_component<MeshRenderer>(Entity) const;
    template bool Scene::has_component<MeshRenderer>(Entity) const;
    template Light& Scene::add_component<Light>(Entity);
    template Light* Scene::get_component<Light>(Entity);
    template bool Scene::remove_component<Light>(Entity) const;
    template bool Scene::has_component<Light>(Entity) const;

    SceneAppComponent::SceneAppComponent()
        : _scene(std::make_unique<Scene>()) {
    }

    SceneAppComponent::~SceneAppComponent() = default;

    void SceneAppComponent::init(App &app) {
        _app = &app;
        _scene->init(app);
        _scene->set_delegate(this);
    }

    void SceneAppComponent::shutdown() {
        if (_scene) {
            _scene->set_delegate(nullptr);
            _scene->shutdown();
        }
        _app = nullptr;
    }

    Scene *SceneAppComponent::get_scene() {
        return _scene.get();
    }

    const Scene *SceneAppComponent::get_scene() const {
        return _scene.get();
    }

    void SceneAppComponent::set_auto_update(bool enabled) {
        _auto_update = enabled;
    }

    bool SceneAppComponent::get_auto_update() const {
        return _auto_update;
    }

    void SceneAppComponent::set_auto_render_reset(bool enabled) {
        _auto_render_reset = enabled;
    }

    bool SceneAppComponent::get_auto_render_reset() const {
        return _auto_render_reset;
    }
}
