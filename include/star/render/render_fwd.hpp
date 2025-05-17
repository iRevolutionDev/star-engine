#pragma once

#include "star/export.hpp"
#include <cstdint>

namespace star {
    class Renderer;
    class ForwardRenderer;
    class DeferredRenderer;
    class SceneRenderer;
    class DebugRenderer;
    class UIRenderer;

    enum class RendererType : uint8_t {
        Forward,
        Deferred,
        Custom
    };
}
