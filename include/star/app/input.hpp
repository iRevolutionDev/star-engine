#pragma once

#include "star/export.hpp"
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace star {
    class Keyboard;
    class Mouse;
    class GameController;

    using UtfChar = uint32_t;

    enum class KeyboardKey {
        Unknown = SDL_SCANCODE_UNKNOWN,
        A = SDL_SCANCODE_A,
        B = SDL_SCANCODE_B,
        C = SDL_SCANCODE_C,
        Space = SDL_SCANCODE_SPACE,
        Escape = SDL_SCANCODE_ESCAPE,
        Enter = SDL_SCANCODE_RETURN,
        Tab = SDL_SCANCODE_TAB,
        Backspace = SDL_SCANCODE_BACKSPACE,
        F1 = SDL_SCANCODE_F1,
        F2 = SDL_SCANCODE_F2,
        F3 = SDL_SCANCODE_F3,
        F4 = SDL_SCANCODE_F4,
        F5 = SDL_SCANCODE_F5,
        F6 = SDL_SCANCODE_F6,
        F7 = SDL_SCANCODE_F7,
        F8 = SDL_SCANCODE_F8,
        F9 = SDL_SCANCODE_F9,
        F10 = SDL_SCANCODE_F10,
        F11 = SDL_SCANCODE_F11,
        F12 = SDL_SCANCODE_F12,
    };

    enum class KeyboardModifier {
        None = 0,
        Shift = SDL_KMOD_SHIFT,
        Ctrl = SDL_KMOD_CTRL,
        Alt = SDL_KMOD_ALT,
        Super = SDL_KMOD_GUI,
        CapsLock = SDL_KMOD_CAPS,
        NumLock = SDL_KMOD_NUM
    };

    class STAR_EXPORT KeyboardModifiers {
    public:
        KeyboardModifiers() = default;

        explicit KeyboardModifiers(uint16_t sdl_modifiers);

        bool has(KeyboardModifier modifier) const;

        bool has_any(std::initializer_list<KeyboardModifier> modifiers) const;

        bool has_all(std::initializer_list<KeyboardModifier> modifiers) const;

        uint16_t to_sdl_modifiers() const;

    private:
        uint16_t _modifiers{0};
    };

    enum class MouseButton {
        Left = SDL_BUTTON_LEFT,
        Middle = SDL_BUTTON_MIDDLE,
        Right = SDL_BUTTON_RIGHT,
        X1 = SDL_BUTTON_X1,
        X2 = SDL_BUTTON_X2
    };

    class STAR_EXPORT IKeyboardListener {
    public:
        virtual ~IKeyboardListener() = default;

        virtual void on_keyboard_key(KeyboardKey key, const KeyboardModifiers &modifiers, bool down) {
        }

        virtual void on_keyboard_char(const UtfChar &chr) {
        }
    };

    template<typename T>
    class STAR_EXPORT ITypeKeyboardListener : public IKeyboardListener {
    public:
        static constexpr auto type_name() { return typeid(T).name(); }
        virtual ~ITypeKeyboardListener() = default;
        virtual void on_keyboard_key(KeyboardKey key, const KeyboardModifiers& modifiers, bool down) = 0;
    };

    class STAR_EXPORT IMouseListener {
    public:
        virtual ~IMouseListener() = default;

        virtual void on_mouse_active(bool active) {
        }

        virtual void on_mouse_position_change(const glm::vec2 &delta, const glm::vec2 &absolute) {
        }

        virtual void on_mouse_scroll_change(const glm::vec2 &delta, const glm::vec2 &absolute) {
        }

        virtual void on_mouse_button(MouseButton button, bool down) {
        }
    };

    template<typename T>
    class STAR_EXPORT ITypeMouseListener : public IMouseListener {
    public:
        static constexpr auto type_name() { return typeid(T).name(); }
    };

    class STAR_EXPORT IControllerListener {
    public:
        virtual ~IControllerListener() = default;

        virtual void on_controller_connected(int controller_id) {
        }

        virtual void on_controller_disconnected(int controller_id) {
        }

        virtual void on_controller_button(int controller_id, int button, bool down) {
        }

        virtual void on_controller_axis(int controller_id, int axis, float value) {
        }
    };

    template<typename T>
    class STAR_EXPORT ITypeControllerListener : public IControllerListener {
    public:
        static constexpr auto type_name() { return typeid(T).name(); }
    };

    class STAR_EXPORT Keyboard {
    public:
        Keyboard();

        ~Keyboard();

        bool is_key_down(KeyboardKey key) const;

        KeyboardModifiers get_modifiers() const;

        void add_listener(IKeyboardListener &listener);

        bool remove_listener(const IKeyboardListener &listener);

        void process_key_event(KeyboardKey key, const KeyboardModifiers &modifiers, bool down);

        void process_char_event(const UtfChar &chr);

    private:
        class KeyboardImpl;
        std::unique_ptr<KeyboardImpl> _impl;
    };

    class STAR_EXPORT Mouse {
    public:
        Mouse();

        ~Mouse();

        bool is_button_down(MouseButton button) const;

        glm::vec2 get_position() const;

        glm::vec2 get_scroll() const;

        void set_visible(bool visible);

        bool is_visible() const;

        void set_captured(bool captured);

        bool is_captured() const;

        void add_listener(IMouseListener &listener);

        bool remove_listener(const IMouseListener &listener);

        void process_motion_event(const glm::vec2 &position);

        void process_scroll_event(const glm::vec2 &scroll);

        void process_button_event(MouseButton button, bool down);

    private:
        class MouseImpl;
        std::unique_ptr<MouseImpl> _impl;
    };

    class STAR_EXPORT GameController {
    public:
        GameController();

        ~GameController();

        void refresh_controllers();

        size_t get_controller_count() const;

        bool is_button_down(int controller_id, int button) const;

        float get_axis_value(int controller_id, int axis) const;

        void add_listener(IControllerListener &listener);

        bool remove_listener(const IControllerListener &listener);

        void process_controller_connection_event(int controller_id, bool connected);

        void process_controller_button_event(int controller_id, int button, bool down);

        void process_controller_axis_event(int controller_id, int axis, float value);

    private:
        class GameControllerImpl;
        std::unique_ptr<GameControllerImpl> _impl;
    };

    class STAR_EXPORT Input {
    public:
        Input();

        ~Input();

        Keyboard &get_keyboard();

        const Keyboard &get_keyboard() const;

        Mouse &get_mouse();

        const Mouse &get_mouse() const;

        GameController &get_controller();

        const GameController &get_controller() const;

        void process_event(const SDL_Event &event);

    private:
        std::unique_ptr<Keyboard> _keyboard;
        std::unique_ptr<Mouse> _mouse;
        std::unique_ptr<GameController> _controller;
    };
}
