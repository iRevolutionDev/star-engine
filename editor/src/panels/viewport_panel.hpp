#pragma once

#include "editor_panel.hpp"
#include <bgfx/bgfx.h>

namespace star::editor {
    class ViewportPanel : public EditorPanel {
    public:
        explicit ViewportPanel(EditorContext &context);

        void on_imgui_render() override;

        bgfx::ViewId get_view_id() const { return _view_id; }
        void set_view_id(const bgfx::ViewId view_id) { _view_id = view_id; }

    private:
        bgfx::ViewId _view_id;
        bgfx::TextureHandle _render_texture;
        bool _texture_initialized = false;
        uint32_t _viewport_width = 0;
        uint32_t _viewport_height = 0;
    };
}
