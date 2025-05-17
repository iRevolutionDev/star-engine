#include "star/scene/entity_registry.hpp"
#include "star/scene/entity.hpp"
#include <algorithm>

namespace star {
    EntityRegistry::EntityRegistry() = default;

    EntityRegistry::~EntityRegistry() {
        for (EntityId id: _entities) {
            if (id != Entity::INVALID_ID) {
                Entity entity(id);
                _on_entity_destroyed.emit(id);
            }
        }
    }

    Entity EntityRegistry::create_entity() {
        EntityId id = generate_entity_id();
        Entity entity(id);

        _on_entity_created.emit(id);

        return entity;
    }

    void EntityRegistry::destroy_entity(Entity entity) {
        if (!is_valid(entity)) {
            return;
        }

        EntityId id = entity.id();

        _on_entity_destroyed.emit(id);

        for (auto &[component_id, storage]: _storages) {
            if (storage->has(id)) {
                _on_component_destroyed[component_id].emit(id);

                storage->remove(id);
            }
        }

        recycle_entity_id(id);
    }

    bool EntityRegistry::is_valid(Entity entity) const {
        EntityId id = entity.id();

        if (id >= _entities.size()) {
            return false;
        }

        return _entities[id] != Entity::INVALID_ID;
    }

    EntityId EntityRegistry::generate_entity_id() {
        EntityId id;

        if (!_free_entities.empty()) {
            id = _free_entities.back();
            _free_entities.pop_back();
            _entities[id] = id;
        } else {
            id = _next_entity_id++;
            _entities.push_back(id);
        }

        return id;
    }

    void EntityRegistry::recycle_entity_id(EntityId id) {
        if (id < _entities.size()) {
            _entities[id] = Entity::INVALID_ID;
            _free_entities.push_back(id);
        }
    }
}
