#if defined(_WIN32)
#define NOMINMAX
#endif

#include <star/app/app.hpp>
#include <star/app/imgui_component.hpp>
#include <star/app/window.hpp>
#include <star/app/input.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <string>
#include <glm/gtx/string_cast.hpp>

using namespace star;

class ImguiSampleApp final : public IAppDelegate, public IImguiRenderer {
public:
    explicit ImguiSampleApp(App &app)
        : _app(app) {
    }

    void init() override {
        spdlog::info("Initializing ImGui sample");

        _app.set_debug_flag(BGFX_DEBUG_TEXT);
        const auto &imgui = _app.add_component<ImGuiComponent>(*this);

        ImGui::SetCurrentContext(imgui.get_context());

        _app.get_window().set_title("Star Engine - ImGui Sample");

        spdlog::info("ImGui sample initialized");
    }

    void shutdown() override {
        spdlog::info("Shutting down ImGui sample");
    }

    void imgui_setup() override {
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        spdlog::info("ImGui setup complete");
    }

    void imgui_render() override {
        render_main_window();
        render_input_window();
        render_metrics_window();
    }

    void update(float delta_time) override {
        auto &mouse = _app.get_input().get_mouse();
        const auto velocity = mouse.get_velocity();

        _mouse_velocity_max.x = std::max(_mouse_velocity_max.x, std::abs(velocity.x));
        _mouse_velocity_max.y = std::max(_mouse_velocity_max.y, std::abs(velocity.y));

        const auto scroll = mouse.get_scroll();
        _mouse_scroll_max.x = std::max(_mouse_scroll_max.x, std::abs(scroll.x));
        _mouse_scroll_max.y = std::max(_mouse_scroll_max.y, std::abs(scroll.y));
    }

private:
    void render_main_window() {
        ImGui::Begin("Star Engine - ImGui Sample");

        ImGui::Text("Welcome to the Star Engine ImGui integration!");
        ImGui::Separator();

        ImGui::Text("This sample demonstrates how to use ImGui with Star Engine");

        ImGui::Spacing();

        // ImGui::InputText("Text Input", &_text);

        ImGui::Spacing();
        if (ImGui::ColorEdit4("Background Color", &_background_color.x)) {
            _app.set_clear_color(_background_color);
        }

        ImGui::Spacing();

        if (ImGui::Button("Reset")) {
            _text = "Hello Star Engine!";
            _background_color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            _app.set_clear_color(_background_color);
        }

        ImGui::End();
    }

    void render_input_window() {
        ImGui::Begin("Input Information");

        auto &mouse = _app.get_input().get_mouse();
        const auto pos = mouse.get_position();
        const auto velocity = mouse.get_velocity();
        const auto scroll = mouse.get_scroll();

        ImGui::Text("Mouse Position: (%.1f, %.1f)", pos.x, pos.y);
        ImGui::Text("Mouse Velocity: (%.1f, %.1f)", velocity.x, velocity.y);
        ImGui::Text("Mouse Velocity Max: (%.1f, %.1f)", _mouse_velocity_max.x, _mouse_velocity_max.y);

        ImGui::Spacing();

        ImGui::Text("Mouse Scroll: (%.1f, %.1f)", scroll.x, scroll.y);
        ImGui::Text("Mouse Scroll Max: (%.1f, %.1f)", _mouse_scroll_max.x, _mouse_scroll_max.y);

        ImGui::Spacing();

        ImGui::Text("Left Button: %s", mouse.is_button_down(MouseButton::Left) ? "DOWN" : "UP");
        ImGui::Text("Right Button: %s", mouse.is_button_down(MouseButton::Right) ? "DOWN" : "UP");
        ImGui::Text("Middle Button: %s", mouse.is_button_down(MouseButton::Middle) ? "DOWN" : "UP");

        ImGui::Spacing();

        if (ImGui::Button("Reset Max Values")) {
            _mouse_velocity_max = glm::vec2(0);
            _mouse_scroll_max = glm::vec2(0);
        }

        ImGui::End();
    }

    void render_metrics_window() const {
        ImGui::Begin("Performance Metrics");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        const auto &win = _app.get_window();
        const auto size = win.get_size();
        ImGui::Text("Window Size: %u x %u", size.x, size.y);

        ImGui::End();
    }

    App &_app;
    std::string _text = "Hello Star Engine!";
    glm::vec4 _background_color{0.3f, 0.3f, 0.3f, 1.0f};
    glm::vec2 _mouse_velocity_max{0};
    glm::vec2 _mouse_scroll_max{0};
};

STAR_RUN_APP(ImguiSampleApp);
