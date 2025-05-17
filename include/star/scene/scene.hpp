#pragma once

#include "star/export.hpp"
#include "star/app/app_fwd.hpp"
#include "entity.hpp"
#include "star/app/app_component.hpp"

namespace star {
    class Scene;
    class Transform;
    class Entity;

    class ISceneComponent;
    class ISceneDelegate;

    class STAR_EXPORT ISceneDelegate {
    public:
        virtual ~ISceneDelegate() = default;

        virtual void on_scene_updated(float delta_time) {
        }

        virtual void on_entity_created(Entity entity) {
        }

        virtual void on_entity_destroyed(Entity entity) {
        }
    };

    class STAR_EXPORT ISceneComponent {
    public:
        virtual ~ISceneComponent() = default;

        virtual void init(Scene &scene, App &app) {
        }

        virtual void shutdown() {
        }

        virtual void update(float delta_time) {
        }

        virtual bgfx::ViewId render_reset(bgfx::ViewId view_id) { return view_id; }

        virtual size_t get_scene_component_type() const { return 0; }
        virtual std::string get_scene_component_name() const { return "ISceneComponent"; }
    };

    template<typename T>
    class STAR_EXPORT ITypeSceneComponent : public ISceneComponent {
    public:
        size_t get_scene_component_type() const override {
            return typeid(T).hash_code();
        }

        std::string get_scene_component_name() const override {
            return typeid(T).name();
        }
    };

    class STAR_EXPORT Scene {
    public:
        Scene();

        ~Scene();

        void init(App &app) const;

        void shutdown();

        void update(float delta_time);

        bgfx::ViewId render_reset(bgfx::ViewId view_id);

        void set_paused(bool paused) const;

        bool is_paused() const;

        void set_name(const std::string &name);

        const std::string &get_name() const;

        template<typename T, typename... Args>
        T &add_scene_component(Args &&... args) {
            static_assert(std::is_base_of_v<ISceneComponent, T>, "T must be derived from ISceneComponent");
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T &ref = *component;
            add_scene_component_impl(std::move(component));
            return ref;
        }

        template<typename T>
        T *get_scene_component() {
            return static_cast<T *>(get_scene_component_impl(typeid(T).hash_code()));
        }

        template<typename T>
        bool remove_scene_component() {
            return remove_scene_component_impl(typeid(T).hash_code());
        }

        Entity create_entity();

        void destroy_entity(Entity entity);

        bool is_valid_entity(Entity entity) const;

        void set_delegate(ISceneDelegate *delegate);        ISceneDelegate *get_delegate() const;

        template<typename T, typename... Args>
        T &add_component(Entity entity, Args &&... args);

        template<typename T>
        T *get_component(Entity entity);

        template<typename T>
        bool remove_component(Entity entity) const;

        template<typename T>
        bool has_component(Entity entity) const;
    private:
        void add_scene_component_impl(std::unique_ptr<ISceneComponent> &&component);

        ISceneComponent *get_scene_component_impl(size_t type_hash);

        bool remove_scene_component_impl(size_t type_hash);

        class SceneImpl;
        std::unique_ptr<SceneImpl> _impl;
    };

    class STAR_EXPORT SceneAppComponent : public ITypeAppComponent<SceneAppComponent>, public ISceneDelegate {
    public:
        SceneAppComponent();

        ~SceneAppComponent();

        void init(App &app) override;

        void shutdown() override;

        Scene *get_scene();

        const Scene *get_scene() const;

        void set_auto_update(bool enabled);

        bool get_auto_update() const;

        void set_auto_render_reset(bool enabled);

        bool get_auto_render_reset() const;

    private:
        std::unique_ptr<Scene> _scene;
        App *_app{nullptr};
        bool _auto_update{true};
        bool _auto_render_reset{true};
    };
}
