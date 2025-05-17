#pragma once

#include "star/export.hpp"
#include "star/scene/entity.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <type_traits>
#include <optional>

namespace star {
    class App;
    class Scene;

    class STAR_EXPORT IComponentStorage {
    public:
        virtual ~IComponentStorage() = default;

        virtual void remove(EntityId entity) = 0;

        virtual bool has(EntityId entity) const = 0;
    };

    template<typename T>
    class ComponentStorage : public IComponentStorage {
    public:
        ComponentStorage() = default;

        ~ComponentStorage() override = default;

        ComponentStorage(const ComponentStorage &) = delete;

        ComponentStorage &operator=(const ComponentStorage &) = delete;

        template<typename... Args>
        T &add(EntityId entity, Args &&... args) {
            auto it = _components.find(entity);
            if (it != _components.end()) {
                it->second = std::make_unique<T>(std::forward<Args>(args)...);
                return *it->second;
            }

            auto result = _components.emplace(entity, std::make_unique<T>(std::forward<Args>(args)...));
            return *result.first->second;
        }

        T *get(EntityId entity) {
            auto it = _components.find(entity);
            return (it != _components.end()) ? it->second.get() : nullptr;
        }

        const T *get(EntityId entity) const {
            auto it = _components.find(entity);
            return (it != _components.end()) ? it->second.get() : nullptr;
        }

        void remove(EntityId entity) override {
            _components.erase(entity);
        }

        bool has(EntityId entity) const override {
            return _components.find(entity) != _components.end();
        }

        class Iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<EntityId, T &>;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

            Iterator(typename std::unordered_map<EntityId, std::unique_ptr<T> >::iterator it)
                : _it(it) {
            }

            value_type operator*() const {
                return {_it->first, *_it->second};
            }

            Iterator &operator++() {
                ++_it;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const Iterator &other) const {
                return _it == other._it;
            }

            bool operator!=(const Iterator &other) const {
                return _it != other._it;
            }

        private:
            typename std::unordered_map<EntityId, std::unique_ptr<T> >::iterator _it;
        };

        class View {
        public:
            View(ComponentStorage &storage) : _storage(storage) {
            }

            Iterator begin() {
                return Iterator(_storage._components.begin());
            }

            Iterator end() {
                return Iterator(_storage._components.end());
            }

            size_t size() const {
                return _storage._components.size();
            }

            bool empty() const {
                return _storage._components.empty();
            }

        private:
            ComponentStorage &_storage;
        };

        View each() {
            return View(*this);
        }

    private:
        std::unordered_map<EntityId, std::unique_ptr<T> > _components;
    };

    using EntityEventFn = std::function<void(EntityId)>;
    using ComponentEventFn = std::function<void(EntityId, ComponentId)>;

    class STAR_EXPORT Signal {
    public:
        template<typename Fn>
        void connect(Fn &&fn) {
            _callbacks.push_back(std::forward<Fn>(fn));
        }

        template<typename... Args>
        void emit(Args &&... args) const {
            for (const auto &callback: _callbacks) {
                callback(std::forward<Args>(args)...);
            }
        }

    private:
        std::vector<std::function<void(EntityId)> > _callbacks;
    };

    class STAR_EXPORT EntityRegistry {
    public:
        EntityRegistry();

        ~EntityRegistry();

        Entity create_entity();

        void destroy_entity(Entity entity);

        bool is_valid(Entity entity) const;

        template<typename T, typename... Args>
        T &add_component(Entity entity, Args &&... args) {
            auto storage = get_or_create_storage<T>();
            T &component = storage->add(entity.id(), std::forward<Args>(args)...);

            _on_component_constructed[ComponentTraits<T>::id()].emit(entity.id());

            return component;
        }

        template<typename T>
        T *get_component(const Entity entity) {
            auto storage = get_storage<T>();
            return storage ? storage->get(entity.id()) : nullptr;
        }

        template<typename T>
        const T *get_component(const Entity entity) const {
            auto storage = get_storage<T>();
            return storage ? storage->get(entity.id()) : nullptr;
        }

        template<typename T>
        bool has_component(const Entity entity) const {
            auto storage = get_storage<T>();
            return storage && storage->has(entity.id());
        }

        template<typename T>
        bool remove_component(const Entity entity) {
            auto storage = get_storage<T>();
            if (storage && storage->has(entity.id())) {
                _on_component_destroyed[ComponentTraits<T>::id()].emit(entity.id());

                storage->remove(entity.id());
                return true;
            }
            return false;
        }

        template<typename T>
        typename ComponentStorage<T>::View get_view() {
            auto storage = get_or_create_storage<T>();
            return storage->each();
        }

        Signal &on_entity_created() { return _on_entity_created; }
        Signal &on_entity_destroyed() { return _on_entity_destroyed; }

        template<typename T>
        Signal &on_component_constructed() {
            return _on_component_constructed[ComponentTraits<T>::id()];
        }

        template<typename T>
        Signal &on_component_destroyed() {
            return _on_component_destroyed[ComponentTraits<T>::id()];
        }

    private:
        template<typename T>
        ComponentStorage<T> *get_or_create_storage() {
            ComponentId id = ComponentTraits<T>::id();
            auto it = _storages.find(id);
            if (it == _storages.end()) {
                auto storage = std::make_unique<ComponentStorage<T> >();
                auto *ptr = storage.get();
                _storages[id] = std::move(storage);
                return ptr;
            }
            return static_cast<ComponentStorage<T> *>(it->second.get());
        }

        template<typename T>
        ComponentStorage<T> *get_storage() {
            ComponentId id = ComponentTraits<T>::id();
            auto it = _storages.find(id);
            return (it != _storages.end()) ? static_cast<ComponentStorage<T> *>(it->second.get()) : nullptr;
        }

        template<typename T>
        const ComponentStorage<T> *get_storage() const {
            const ComponentId id = ComponentTraits<T>::id();
            const auto it = _storages.find(id);
            return it != _storages.end() ? static_cast<const ComponentStorage<T> *>(it->second.get()) : nullptr;
        }

        EntityId generate_entity_id();

        void recycle_entity_id(EntityId id);

        std::vector<EntityId> _entities;
        std::vector<EntityId> _free_entities;
        EntityId _next_entity_id{0};

        std::unordered_map<ComponentId, std::unique_ptr<IComponentStorage> > _storages;

        Signal _on_entity_created;
        Signal _on_entity_destroyed;
        std::unordered_map<ComponentId, Signal> _on_component_constructed;
        std::unordered_map<ComponentId, Signal> _on_component_destroyed;
    };
}
