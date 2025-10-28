#pragma once

#include "star/export.hpp"
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <memory>

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
        D = SDL_SCANCODE_D,
        E = SDL_SCANCODE_E,
        F = SDL_SCANCODE_F,
        G = SDL_SCANCODE_G,
        H = SDL_SCANCODE_H,
        I = SDL_SCANCODE_I,
        J = SDL_SCANCODE_J,
        K = SDL_SCANCODE_K,
        L = SDL_SCANCODE_L,
        M = SDL_SCANCODE_M,
        N = SDL_SCANCODE_N,
        O = SDL_SCANCODE_O,
        P = SDL_SCANCODE_P,
        Q = SDL_SCANCODE_Q,
        R = SDL_SCANCODE_R,
        S = SDL_SCANCODE_S,
        T = SDL_SCANCODE_T,
        U = SDL_SCANCODE_U,
        V = SDL_SCANCODE_V,
        W = SDL_SCANCODE_W,
        X = SDL_SCANCODE_X,
        Y = SDL_SCANCODE_Y,
        Z = SDL_SCANCODE_Z,
        Num0 = SDL_SCANCODE_0,
        Num1 = SDL_SCANCODE_1,
        Num2 = SDL_SCANCODE_2,
        Num3 = SDL_SCANCODE_3,
        Num4 = SDL_SCANCODE_4,
        Num5 = SDL_SCANCODE_5,
        Num6 = SDL_SCANCODE_6,
        Num7 = SDL_SCANCODE_7,
        Num8 = SDL_SCANCODE_8,
        Num9 = SDL_SCANCODE_9,
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
        Left = SDL_SCANCODE_LEFT,
        Right = SDL_SCANCODE_RIGHT,
        Up = SDL_SCANCODE_UP,
        Down = SDL_SCANCODE_DOWN,
        PageUp = SDL_SCANCODE_PAGEUP,
        PageDown = SDL_SCANCODE_PAGEDOWN,
        Home = SDL_SCANCODE_HOME,
        End = SDL_SCANCODE_END,
        Insert = SDL_SCANCODE_INSERT,
        Delete = SDL_SCANCODE_DELETE,
        Space = SDL_SCANCODE_SPACE,
        Enter = SDL_SCANCODE_RETURN,
        Escape = SDL_SCANCODE_ESCAPE,
        Tab = SDL_SCANCODE_TAB,
        Backspace = SDL_SCANCODE_BACKSPACE,
        CapsLock = SDL_SCANCODE_CAPSLOCK,
        LeftCtrl = SDL_SCANCODE_LCTRL,
        RightCtrl = SDL_SCANCODE_RCTRL,
        LeftShift = SDL_SCANCODE_LSHIFT,
        RightShift = SDL_SCANCODE_RSHIFT,
        LeftAlt = SDL_SCANCODE_LALT,
        RightAlt = SDL_SCANCODE_RALT,
        LeftSuper = SDL_SCANCODE_LGUI,
        RightSuper = SDL_SCANCODE_RGUI,
        Apostrophe = SDL_SCANCODE_APOSTROPHE,
        Comma = SDL_SCANCODE_COMMA,
        Minus = SDL_SCANCODE_MINUS,
        Period = SDL_SCANCODE_PERIOD,
        Slash = SDL_SCANCODE_SLASH,
        Semicolon = SDL_SCANCODE_SEMICOLON,
        Equal = SDL_SCANCODE_EQUALS,
        LeftBracket = SDL_SCANCODE_LEFTBRACKET,
        Backslash = SDL_SCANCODE_BACKSLASH,
        RightBracket = SDL_SCANCODE_RIGHTBRACKET,
        GraveAccent = SDL_SCANCODE_GRAVE,
        Keypad0 = SDL_SCANCODE_KP_0,
        Keypad1 = SDL_SCANCODE_KP_1,
        Keypad2 = SDL_SCANCODE_KP_2,
        Keypad3 = SDL_SCANCODE_KP_3,
        Keypad4 = SDL_SCANCODE_KP_4,
        Keypad5 = SDL_SCANCODE_KP_5,
        Keypad6 = SDL_SCANCODE_KP_6,
        Keypad7 = SDL_SCANCODE_KP_7,
        Keypad8 = SDL_SCANCODE_KP_8,
        Keypad9 = SDL_SCANCODE_KP_9,
        KeypadDecimal = SDL_SCANCODE_KP_DECIMAL,
        KeypadDivide = SDL_SCANCODE_KP_DIVIDE,
        KeypadMultiply = SDL_SCANCODE_KP_MULTIPLY,
        KeypadSubtract = SDL_SCANCODE_KP_MINUS,
        KeypadAdd = SDL_SCANCODE_KP_PLUS,
        KeypadEnter = SDL_SCANCODE_KP_ENTER,
        KeypadEqual = SDL_SCANCODE_KP_EQUALS,
        Menu = SDL_SCANCODE_MENU,
        PrintScreen = SDL_SCANCODE_PRINTSCREEN,
        ScrollLock = SDL_SCANCODE_SCROLLLOCK,
        Pause = SDL_SCANCODE_PAUSE,
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

    enum class GamepadButton {
        A = SDL_GAMEPAD_BUTTON_SOUTH,
        B = SDL_GAMEPAD_BUTTON_EAST,
        X = SDL_GAMEPAD_BUTTON_WEST,
        Y = SDL_GAMEPAD_BUTTON_NORTH,
        Back = SDL_GAMEPAD_BUTTON_BACK,
        Guide = SDL_GAMEPAD_BUTTON_GUIDE,
        Start = SDL_GAMEPAD_BUTTON_START,
        LeftStick = SDL_GAMEPAD_BUTTON_LEFT_STICK,
        RightStick = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        LeftShoulder = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
        RightShoulder = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        DPadUp = SDL_GAMEPAD_BUTTON_DPAD_UP,
        DPadDown = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        DPadLeft = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
        DPadRight = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
        Misc1 = SDL_GAMEPAD_BUTTON_MISC1,
        Paddle1 = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,
        Paddle2 = SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,
        Paddle3 = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,
        Paddle4 = SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,
        Touchpad = SDL_GAMEPAD_BUTTON_TOUCHPAD,
    };

    enum class GamepadAxis {
        LeftX = SDL_GAMEPAD_AXIS_LEFTX,
        LeftY = SDL_GAMEPAD_AXIS_LEFTY,
        RightX = SDL_GAMEPAD_AXIS_RIGHTX,
        RightY = SDL_GAMEPAD_AXIS_RIGHTY,
        LeftTrigger = SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
        RightTrigger = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
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

        virtual void on_keyboard_key(KeyboardKey key, const KeyboardModifiers &modifiers, bool down) = 0;
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

        glm::vec2 get_velocity() const;

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
