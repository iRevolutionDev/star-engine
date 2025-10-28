#pragma once

#include "star/app/app.hpp"
#include "star/app/imgui_component.hpp"
#include "star/scene/scene.hpp"
#include "star/scene/camera.hpp"
#include <memory>
#include <vector>

namespace star::editor {
    class EditorPanel;
    class EditorContext;

    class EditorApp final : public IAppDelegate, public IImguiRenderer, public IKeyboardListener {
    public:
        explicit EditorApp(App &app);

        ~EditorApp() override;

        void init() override;

        void shutdown() override;

        void update(float delta_time) override;

        void pre_render() override;

        void post_render() override;

        void render() const override;

        bgfx::ViewId render_reset(bgfx::ViewId viewId) override;

        void imgui_setup() override;

        void imgui_render() override;

        App &get_app() const { return _app; }
        Scene *get_active_scene() const { return _active_scene; }
        EditorContext &get_context() const { return *_context; }

        void new_scene();

        void load_scene(const std::string &path);

        void save_scene(const std::string &path);

    private:
        void setup_panels();

        void setup_editor_camera();

        void create_test_objects() const;

        void render_main_menu_bar();

        static void render_dockspace();

        void on_keyboard_key(KeyboardKey key, const KeyboardModifiers &modifiers, bool down) override;

        App &_app;
        SceneAppComponent *_scene_component = nullptr;
        Scene *_active_scene = nullptr;
        Entity _editor_camera_entity;

        std::unique_ptr<EditorContext> _context;
        std::vector<std::unique_ptr<EditorPanel> > _panels;

        bool _show_demo_window = false;
        bool _show_stats = false;
        bool _show_debug = false;
    };
}
