#pragma once

#include "editor_panel.hpp"
#include <bgfx/bgfx.h>

namespace star::editor
{
    class ViewportPanel : public EditorPanel
    {
    public:
        explicit ViewportPanel(EditorContext &context);
        ~ViewportPanel() override;

        void on_imgui_render() override;

        bgfx::ViewId get_view_id() const { return _view_id; }
        void set_view_id(const bgfx::ViewId view_id) { _view_id = view_id; }

        bgfx::FrameBufferHandle get_framebuffer() const { return _framebuffer; }
        uint32_t get_width() const { return _viewport_width; }
        uint32_t get_height() const { return _viewport_height; }

    private:
        bgfx::ViewId _view_id;
        bgfx::TextureHandle _render_texture;
        bgfx::FrameBufferHandle _framebuffer;
        bool _texture_initialized = false;
        uint32_t _viewport_width = 0;
        uint32_t _viewport_height = 0;
    };
}
