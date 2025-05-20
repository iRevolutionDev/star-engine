#pragma once

#include <entt/entt.hpp>

namespace star {
    using Entity = entt::entity;
    using EntityRegistry = entt::registry;
    using EntitySparseSet = entt::sparse_set;
    using EntityStorage = entt::storage<Entity>;
    using SceneSnapshot = entt::snapshot;
    using SceneSnapshotLoader = entt::snapshot_loader;
    using SceneContinuousLoader = entt::continuous_loader;
}
