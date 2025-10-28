#pragma once

#include <string>

namespace star::editor {
    class EditorContext;

    class EditorPanel {
    public:
        explicit EditorPanel(const std::string &name, EditorContext &context)
            : _name(name), _context(context), _is_open(true) {}

        virtual ~EditorPanel() = default;

        virtual void on_imgui_render() = 0;

        const std::string &get_name() const { return _name; }
        bool is_open() const { return _is_open; }
        void set_open(const bool open) { _is_open = open; }

    protected:
        EditorContext &get_context() const { return _context; }

    private:
        std::string _name;
        EditorContext &_context;
        bool _is_open;
    };
}
