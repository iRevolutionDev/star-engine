#pragma once

#include "editor_panel.hpp"
#include "star/scene/entity_registry.hpp"
#include <string>

namespace star::editor {
    class SceneHierarchyPanel : public EditorPanel {
    public:
        explicit SceneHierarchyPanel(EditorContext &context);

        void on_imgui_render() override;

    private:
        void draw_entity_node(Entity entity, const std::string &name) const;
        void draw_context_menu() const;
    };
}
