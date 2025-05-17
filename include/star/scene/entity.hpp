#pragma once

#include "star/export.hpp"
#include <cstdint>
#include <type_traits>

namespace star {
    using EntityId = uint32_t;

    class STAR_EXPORT Entity {
    public:
        static constexpr EntityId INVALID_ID = UINT32_MAX;

        Entity() : _id(INVALID_ID) {
        }

        explicit Entity(EntityId id) : _id(id) {
        }

        Entity(const Entity &) = default;

        Entity &operator=(const Entity &) = default;

        bool operator==(EntityId uint32) const;

        Entity(Entity &&) = default;

        Entity &operator=(Entity &&) = default;

        Entity &operator=(const uint32_t id) {
            _id = id;
            return *this;
        }


        bool operator==(const Entity &other) const { return _id == other._id; }
        bool operator!=(const Entity &other) const { return _id != other._id; }
        bool operator<(const Entity &other) const { return _id < other._id; }

        bool is_valid() const { return _id != INVALID_ID; }

        EntityId id() const { return _id; }

        void reset() { _id = INVALID_ID; }

    private:
        EntityId _id;
    };

    // static constexpr auto NULL_ENTITY = Entity(Entity::INVALID_ID);

    using ComponentId = uint32_t;

    template<typename T>
    struct ComponentTraits {
        static ComponentId id() {
            static const ComponentId id = next_id();
            return id;
        }

        static const char *name() {
            return typeid(T).name();
        }

    private:
        static ComponentId next_id() {
            static ComponentId next = 0;
            return next++;
        }
    };
}
