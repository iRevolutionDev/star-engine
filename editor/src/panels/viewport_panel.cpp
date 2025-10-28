#include "viewport_panel.hpp"
#include "editor_context.hpp"
#include <imgui.h>

namespace star::editor {
    ViewportPanel::ViewportPanel(EditorContext &context)
        : EditorPanel("Viewport", context), _view_id(0) {
        _render_texture = BGFX_INVALID_HANDLE;
    }

    void ViewportPanel::on_imgui_render() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        const auto viewport_min = ImGui::GetWindowContentRegionMin();
        const auto viewport_max = ImGui::GetWindowContentRegionMax();
        auto viewport_offset = ImGui::GetWindowPos();

        const auto viewport_size = ImVec2(
            viewport_max.x - viewport_min.x,
            viewport_max.y - viewport_min.y
        );

        get_context().set_viewport_hovered(ImGui::IsWindowHovered());
        get_context().set_viewport_focused(ImGui::IsWindowFocused());

        if (viewport_size.x > 0 && viewport_size.y > 0) {
            _viewport_width = static_cast<uint32_t>(viewport_size.x);
            _viewport_height = static_cast<uint32_t>(viewport_size.y);

            // TODO: Render scene to texture and display here
            ImGui::Text("Viewport: %.0fx%.0f", viewport_size.x, viewport_size.y);
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
