#pragma once

#include "star/scene/entity_registry.hpp"
#include <optional>

namespace star::editor {
    class EditorApp;

    class EditorContext {
    public:
        explicit EditorContext(EditorApp *app) : _app(app) {
        }

        EditorApp *get_app() const { return _app; }

        void set_selected_entity(Entity entity) { _selected_entity = entity; }
        std::optional<Entity> get_selected_entity() const { return _selected_entity; }
        void clear_selection() { _selected_entity = std::nullopt; }

        bool is_viewport_hovered() const { return _viewport_hovered; }
        void set_viewport_hovered(const bool hovered) { _viewport_hovered = hovered; }

        bool is_viewport_focused() const { return _viewport_focused; }
        void set_viewport_focused(const bool focused) { _viewport_focused = focused; }

    private:
        EditorApp *_app;
        std::optional<Entity> _selected_entity;
        bool _viewport_hovered = false;
        bool _viewport_focused = false;
    };
}
