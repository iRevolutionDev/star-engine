#include "editor_app.hpp"
#include "editor_context.hpp"
#include "panels/scene_hierarchy_panel.hpp"
#include "panels/inspector_panel.hpp"
#include "panels/viewport_panel.hpp"
#include "panels/console_panel.hpp"
#include "star/app/imgui_component.hpp"
#include "star/scene/transform.hpp"
#include "star/render/forward_renderer.hpp"
#include "star/render/scene_renderer.hpp"
#include <imgui.h>
#include <spdlog/spdlog.h>

namespace star::editor {
    EditorApp::EditorApp(App &app)
        : _app(app), _editor_camera_entity() {
        _context = std::make_unique<EditorContext>(this);
    }

    EditorApp::~EditorApp() = default;

    void EditorApp::init() {
        spdlog::info("Initializing Star Engine Editor");

        _app.get_window().set_title("Star Engine Editor");
        _app.get_input().get_keyboard().add_listener(*this);

        if (_show_debug) {
            _app.set_debug_flag(BGFX_DEBUG_TEXT);
        }

        _app.set_debug_flag(BGFX_DEBUG_STATS, _show_stats);

        const auto &imgui = _app.add_component<ImGuiComponent>(*this);
        ImGui::SetCurrentContext(imgui.get_context());

        _scene_component = &_app.add_component<SceneAppComponent>();
        _active_scene = _scene_component->get_scene();
        _active_scene->set_name("EditorScene");

        setup_editor_camera();

        _active_scene->add_scene_component<SceneRendererComponent>();

        setup_panels();

        spdlog::info("Star Engine Editor initialized successfully");
    }

    void EditorApp::shutdown() {
        spdlog::info("Shutting down Star Engine Editor");
        _panels.clear();
    }

    void EditorApp::update(const float delta_time) {
        if (_context->is_viewport_focused()) {
        }
    }

    void EditorApp::pre_render() {
    }

    void EditorApp::post_render() {
    }

    void EditorApp::render() const {
    }

    bgfx::ViewId EditorApp::render_reset(const bgfx::ViewId viewId) {
        return viewId;
    }

    void EditorApp::imgui_setup() {
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.0f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.0f);
    }

    void EditorApp::imgui_render() {
        render_dockspace();

        render_main_menu_bar();

        for (const auto &panel: _panels) {
            if (panel->is_open()) {
                panel->on_imgui_render();
            }
        }

        if (_show_demo_window) {
            ImGui::ShowDemoWindow(&_show_demo_window);
        }
    }

    void EditorApp::setup_panels() {
        _panels.push_back(std::make_unique<SceneHierarchyPanel>(*_context));
        _panels.push_back(std::make_unique<InspectorPanel>(*_context));
        _panels.push_back(std::make_unique<ViewportPanel>(*_context));
        _panels.push_back(std::make_unique<ConsolePanel>(*_context));
    }

    void EditorApp::setup_editor_camera() {
        _editor_camera_entity = _active_scene->create_entity();

        auto &transform = _active_scene->add_component<Transform>(_editor_camera_entity);
        transform.set_position(glm::vec3(0.0f, 2.0f, -5.0f));
        transform.look_at(glm::vec3(0.0f, 0.0f, 0.0f));

        auto &camera = _active_scene->add_component<Camera>(_editor_camera_entity);
        camera.set_perspective(60.0f, 0.1f, 1000.0f);

        camera.add_component<ForwardRendererComponent>();
    }

    void EditorApp::render_main_menu_bar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                    new_scene();
                }
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
                    spdlog::warn("Open scene not implemented yet");
                }
                if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                    spdlog::warn("Save scene not implemented yet");
                }
                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                    spdlog::warn("Save scene as not implemented yet");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    //_app.quit();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                for (const auto &panel: _panels) {
                    bool is_open = panel->is_open();
                    if (ImGui::MenuItem(panel->get_name().c_str(), nullptr, &is_open)) {
                        panel->set_open(is_open);
                    }
                }
                ImGui::Separator();
                ImGui::MenuItem("Show Stats", nullptr, &_show_stats);
                ImGui::MenuItem("Show Demo Window", nullptr, &_show_demo_window);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) {
                    // TODO: Show about dialog
                    spdlog::info("Star Engine Editor v0.1.0");
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void EditorApp::render_dockspace() {
        static bool dockspace_open = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspace_open, window_flags);
        ImGui::PopStyleVar(3);

        if (const auto &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            const auto dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::End();
    }

    void EditorApp::on_keyboard_key(const KeyboardKey key, const KeyboardModifiers &modifiers, const bool down) {
        if (!down) return;

        switch (key) {
            case KeyboardKey::F7:
                _show_stats = !_show_stats;
                _app.set_debug_flag(BGFX_DEBUG_STATS, _show_stats);
                break;
            case KeyboardKey::F8:
                _show_debug = !_show_debug;
                _app.set_debug_flag(BGFX_DEBUG_TEXT, _show_debug);
            default:
                break;
        }
    }

    void EditorApp::new_scene() {
        spdlog::info("Creating new scene");

        if (_active_scene) {
            //_active_scene->clear();
        }

        setup_editor_camera();

        _context->clear_selection();
    }

    void EditorApp::load_scene(const std::string &path) {
        spdlog::info("Loading scene: {}", path);
        // TODO: Implement scene loading
    }

    void EditorApp::save_scene(const std::string &path) {
        spdlog::info("Saving scene: {}", path);
        // TODO: Implement scene saving
    }
}
