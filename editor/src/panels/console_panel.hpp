#pragma once

#include "editor_panel.hpp"
#include <vector>
#include <string>

namespace star::editor {
    struct LogMessage {
        enum class Level {
            Info,
            Warning,
            Error
        };

        Level level;
        std::string message;
        std::string timestamp;
    };

    class ConsolePanel : public EditorPanel {
    public:
        explicit ConsolePanel(EditorContext &context);

        void on_imgui_render() override;

        void add_log(LogMessage::Level level, const std::string &message);
        void clear();

    private:
        std::vector<LogMessage> _messages;
        bool _auto_scroll = true;
    };
}
