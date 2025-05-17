#include "star/scene/entity_registry.hpp"
#include "star/render/renderer_components.hpp"
#include "star/scene/camera.hpp"
#include "star/scene/transform.hpp"

namespace star {
    static void register_builtin_components() {
        EntityRegistry registry;

        registry.add_component<Transform>(Entity(0));
        registry.remove_component<Transform>(Entity(0));
        registry.has_component<Transform>(Entity(0));
        registry.get_component<Transform>(Entity(0));

        registry.add_component<Camera>(Entity(0));
        registry.remove_component<Camera>(Entity(0));
        registry.has_component<Camera>(Entity(0));
        registry.get_component<Camera>(Entity(0));

        registry.add_component<MeshRenderer>(Entity(0));
        registry.remove_component<MeshRenderer>(Entity(0));
        registry.has_component<MeshRenderer>(Entity(0));
        registry.get_component<MeshRenderer>(Entity(0));

        registry.add_component<Light>(Entity(0));
        registry.remove_component<Light>(Entity(0));
        registry.has_component<Light>(Entity(0));
        registry.get_component<Light>(Entity(0));
    }

    namespace {
        static const bool registered = (register_builtin_components(), true);
    }
}
