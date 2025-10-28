#include "console_panel.hpp"
#include <imgui.h>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace star::editor {
    ConsolePanel::ConsolePanel(EditorContext &context)
        : EditorPanel("Console", context) {
        add_log(LogMessage::Level::Info, "Console initialized");
    }

    void ConsolePanel::on_imgui_render() {
        ImGui::Begin("Console");

        if (ImGui::Button("Clear")) {
            clear();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &_auto_scroll);

        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto &[level, message, timestamp]: _messages) {
            ImVec4 color;
            const char *prefix = nullptr;

            switch (level) {
                case LogMessage::Level::Info:
                    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    prefix = "[INFO]";
                    break;
                case LogMessage::Level::Warning:
                    color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                    prefix = "[WARN]";
                    break;
                case LogMessage::Level::Error:
                    color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    prefix = "[ERROR]";
                    break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped("%s %s %s", timestamp.c_str(), prefix, message.c_str());
            ImGui::PopStyleColor();
        }

        if (_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::End();
    }

    void ConsolePanel::add_log(const LogMessage::Level level, const std::string &message) {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");

        LogMessage msg;
        msg.level = level;
        msg.message = message;
        msg.timestamp =
                ss.str();

        _messages.push_back(msg);
    }

    void ConsolePanel::clear() {
        _messages.clear();
        add_log(LogMessage::Level::Info, "Console cleared");
    }
}
