#include "star/app/input.hpp"
#include <algorithm>
#include <ranges>
#include <spdlog/spdlog.h>

namespace star {
    KeyboardModifiers::KeyboardModifiers(uint16_t sdl_modifiers)
        : _modifiers(sdl_modifiers) {
    }

    bool KeyboardModifiers::has(KeyboardModifier modifier) const {
        return (_modifiers & static_cast<uint16_t>(modifier)) != 0;
    }

    bool KeyboardModifiers::has_any(std::initializer_list<KeyboardModifier> modifiers) const {
        for (const auto mod: modifiers) {
            if (has(mod)) {
                return true;
            }
        }
        return false;
    }

    bool KeyboardModifiers::has_all(std::initializer_list<KeyboardModifier> modifiers) const {
        for (auto mod: modifiers) {
            if (!has(mod)) {
                return false;
            }
        }
        return true;
    }

    uint16_t KeyboardModifiers::to_sdl_modifiers() const {
        return _modifiers;
    }

    class Keyboard::KeyboardImpl {
    public:
        bool is_key_down(KeyboardKey key) const;

        KeyboardModifiers get_modifiers() const;

        void add_listener(IKeyboardListener &listener);

        bool remove_listener(const IKeyboardListener &listener);

        void process_key_event(KeyboardKey key, const KeyboardModifiers &modifiers, bool down);

        void process_char_event(const UtfChar &chr);

    private:
        std::vector<std::reference_wrapper<IKeyboardListener> > _listeners;
        std::unordered_map<KeyboardKey, bool> _key_states;
        KeyboardModifiers _modifiers{0};
    };

    bool Keyboard::KeyboardImpl::is_key_down(KeyboardKey key) const {
        auto it = _key_states.find(key);
        return (it != _key_states.end()) && it->second;
    }

    KeyboardModifiers Keyboard::KeyboardImpl::get_modifiers() const {
        return _modifiers;
    }

    void Keyboard::KeyboardImpl::add_listener(IKeyboardListener &listener) {
        auto it = std::find_if(_listeners.begin(), _listeners.end(),
                               [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it == _listeners.end()) {
            _listeners.push_back(std::ref(listener));
        }
    }

    bool Keyboard::KeyboardImpl::remove_listener(const IKeyboardListener &listener) {
        auto it = std::find_if(_listeners.begin(), _listeners.end(),
                               [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it != _listeners.end()) {
            _listeners.erase(it);
            return true;
        }

        return false;
    }

    void Keyboard::KeyboardImpl::process_key_event(KeyboardKey key, const KeyboardModifiers &modifiers, bool down) {
        _key_states[key] = down;
        _modifiers = modifiers;

        for (auto &listener: _listeners) {
            listener.get().on_keyboard_key(key, modifiers, down);
        }
    }

    void Keyboard::KeyboardImpl::process_char_event(const UtfChar &chr) {
        for (auto &listener: _listeners) {
            listener.get().on_keyboard_char(chr);
        }
    }

    Keyboard::Keyboard() : _impl(std::make_unique<KeyboardImpl>()) {
    }

    Keyboard::~Keyboard() = default;

    bool Keyboard::is_key_down(KeyboardKey key) const {
        return _impl->is_key_down(key);
    }

    KeyboardModifiers Keyboard::get_modifiers() const {
        return _impl->get_modifiers();
    }

    void Keyboard::add_listener(IKeyboardListener &listener) {
        _impl->add_listener(listener);
    }

    bool Keyboard::remove_listener(const IKeyboardListener &listener) {
        return _impl->remove_listener(listener);
    }

    void Keyboard::process_key_event(KeyboardKey key, const KeyboardModifiers &modifiers, bool down) {
        _impl->process_key_event(key, modifiers, down);
    }

    void Keyboard::process_char_event(const UtfChar &chr) {
        _impl->process_char_event(chr);
    }

    class Mouse::MouseImpl {
    public:
        MouseImpl();

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
        std::vector<std::reference_wrapper<IMouseListener> > _listeners;
        std::unordered_map<MouseButton, bool> _button_states;
        glm::vec2 _position{0.0f};
        glm::vec2 _scroll{0.0f};
        bool _visible{true};
        bool _captured{false};
        SDL_Window *_window{nullptr};
    };

    Mouse::MouseImpl::MouseImpl() {
    }

    bool Mouse::MouseImpl::is_button_down(MouseButton button) const {
        auto it = _button_states.find(button);
        return (it != _button_states.end()) && it->second;
    }

    glm::vec2 Mouse::MouseImpl::get_position() const {
        return _position;
    }

    glm::vec2 Mouse::MouseImpl::get_scroll() const {
        return _scroll;
    }

    void Mouse::MouseImpl::set_visible(bool visible) {
        if (_visible != visible) {
            _visible = visible;
            SDL_SetWindowRelativeMouseMode(_window, visible);
        }
    }

    bool Mouse::MouseImpl::is_visible() const {
        return _visible;
    }

    void Mouse::MouseImpl::set_captured(bool captured) {
        _captured = captured;
    }

    bool Mouse::MouseImpl::is_captured() const {
        return _captured;
    }

    void Mouse::MouseImpl::add_listener(IMouseListener &listener) {
        const auto it = std::ranges::find_if(_listeners,
                                             [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it == _listeners.end()) {
            _listeners.push_back(std::ref(listener));
        }
    }

    bool Mouse::MouseImpl::remove_listener(const IMouseListener &listener) {
        const auto it = std::ranges::find_if(_listeners,
                                             [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it != _listeners.end()) {
            _listeners.erase(it);
            return true;
        }

        return false;
    }

    void Mouse::MouseImpl::process_motion_event(const glm::vec2 &position) {
        glm::vec2 delta = position - _position;
        _position = position;

        for (auto &listener: _listeners) {
            listener.get().on_mouse_position_change(delta, position);
        }
    }

    void Mouse::MouseImpl::process_scroll_event(const glm::vec2 &scroll) {
        _scroll = scroll;

        for (auto &listener: _listeners) {
            listener.get().on_mouse_scroll_change(scroll, _scroll);
        }
    }

    void Mouse::MouseImpl::process_button_event(MouseButton button, bool down) {
        _button_states[button] = down;

        for (auto &listener: _listeners) {
            listener.get().on_mouse_button(button, down);
        }
    }

    glm::vec2 Mouse::MouseImpl::get_velocity() const {
        const auto position = get_position();

        static glm::vec2 last_position = position;
        const glm::vec2 velocity = position - last_position;
        last_position = position;

        return velocity;
    }

    Mouse::Mouse() : _impl(std::make_unique<MouseImpl>()) {
    }

    Mouse::~Mouse() = default;

    bool Mouse::is_button_down(MouseButton button) const {
        return _impl->is_button_down(button);
    }

    glm::vec2 Mouse::get_position() const {
        return _impl->get_position();
    }

    glm::vec2 Mouse::get_scroll() const {
        return _impl->get_scroll();
    }

    void Mouse::set_visible(bool visible) {
        _impl->set_visible(visible);
    }

    bool Mouse::is_visible() const {
        return _impl->is_visible();
    }

    void Mouse::set_captured(bool captured) {
        _impl->set_captured(captured);
    }

    bool Mouse::is_captured() const {
        return _impl->is_captured();
    }

    void Mouse::add_listener(IMouseListener &listener) {
        _impl->add_listener(listener);
    }

    bool Mouse::remove_listener(const IMouseListener &listener) {
        return _impl->remove_listener(listener);
    }

    void Mouse::process_motion_event(const glm::vec2 &position) {
        _impl->process_motion_event(position);
    }

    void Mouse::process_scroll_event(const glm::vec2 &scroll) {
        _impl->process_scroll_event(scroll);
    }

    void Mouse::process_button_event(MouseButton button, bool down) {
        _impl->process_button_event(button, down);
    }

    glm::vec2 Mouse::get_velocity() const {
        return _impl->get_velocity();
    }

    class GameController::GameControllerImpl {
    public:
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
        struct ControllerState {
            std::unordered_map<int, bool> button_states;
            std::unordered_map<int, float> axis_values;
            SDL_Gamepad *handle{nullptr};
            bool connected{false};
        };

        std::vector<std::reference_wrapper<IControllerListener> > _listeners;
        std::unordered_map<int, ControllerState> _controllers;
    };

    void GameController::GameControllerImpl::refresh_controllers() {
        for (auto &state: _controllers | std::views::values) {
            if (state.handle) {
                SDL_CloseGamepad(state.handle);
                state.handle = nullptr;
                state.connected = false;
            }
        }

        // int num_joysticks = SDL_GetNumJoysticks();
        // for (int i = 0; i < num_joysticks; ++i) {
        //     if (SDL_IsGamepad(i)) {
        //         SDL_Gamepad* gamepad = SDL_OpenGamepad(i);
        //         if (gamepad) {
        //             _controllers[i].handle = gamepad;
        //             _controllers[i].connected = true;
        //
        //             // Notify listeners
        //             for (auto& listener : _listeners) {
        //                 listener.get().on_controller_connected(i);
        //             }
        //         }
        //     }
        // }
    }

    size_t GameController::GameControllerImpl::get_controller_count() const {
        size_t count = 0;
        for (const auto &state: _controllers | std::views::values) {
            if (state.connected) {
                count++;
            }
        }
        return count;
    }

    bool GameController::GameControllerImpl::is_button_down(int controller_id, int button) const {
        if (const auto it = _controllers.find(controller_id); it != _controllers.end() && it->second.connected) {
            const auto btn_it = it->second.button_states.find(button);
            return btn_it != it->second.button_states.end() && btn_it->second;
        }
        return false;
    }

    float GameController::GameControllerImpl::get_axis_value(int controller_id, int axis) const {
        auto it = _controllers.find(controller_id);
        if (it != _controllers.end() && it->second.connected) {
            auto axis_it = it->second.axis_values.find(axis);
            return (axis_it != it->second.axis_values.end()) ? axis_it->second : 0.0f;
        }
        return 0.0f;
    }

    void GameController::GameControllerImpl::add_listener(IControllerListener &listener) {
        auto it = std::find_if(_listeners.begin(), _listeners.end(),
                               [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it == _listeners.end()) {
            _listeners.push_back(std::ref(listener));
        }
    }

    bool GameController::GameControllerImpl::remove_listener(const IControllerListener &listener) {
        auto it = std::find_if(_listeners.begin(), _listeners.end(),
                               [&listener](const auto &ref) { return &ref.get() == &listener; });

        if (it != _listeners.end()) {
            _listeners.erase(it);
            return true;
        }

        return false;
    }

    void GameController::GameControllerImpl::process_controller_connection_event(int controller_id, bool connected) {
        auto &state = _controllers[controller_id];

        if (connected) {
            if (!state.connected) {
                state.handle = SDL_OpenGamepad(controller_id);
                state.connected = state.handle != nullptr;

                if (state.connected) {
                    for (auto &listener: _listeners) {
                        listener.get().on_controller_connected(controller_id);
                    }
                }
            }
        } else {
            if (state.connected) {
                if (state.handle) {
                    SDL_CloseGamepad(state.handle);
                    state.handle = nullptr;
                }

                state.connected = false;

                for (auto &listener: _listeners) {
                    listener.get().on_controller_disconnected(controller_id);
                }
            }
        }
    }

    void GameController::GameControllerImpl::process_controller_button_event(int controller_id, int button, bool down) {
        auto it = _controllers.find(controller_id);
        if (it != _controllers.end() && it->second.connected) {
            it->second.button_states[button] = down;

            for (auto &listener: _listeners) {
                listener.get().on_controller_button(controller_id, button, down);
            }
        }
    }

    void GameController::GameControllerImpl::process_controller_axis_event(int controller_id, int axis, float value) {
        auto it = _controllers.find(controller_id);
        if (it != _controllers.end() && it->second.connected) {
            it->second.axis_values[axis] = value;

            for (auto &listener: _listeners) {
                listener.get().on_controller_axis(controller_id, axis, value);
            }
        }
    }

    GameController::GameController() : _impl(std::make_unique<GameControllerImpl>()) {
    }

    GameController::~GameController() = default;

    void GameController::refresh_controllers() {
        _impl->refresh_controllers();
    }

    size_t GameController::get_controller_count() const {
        return _impl->get_controller_count();
    }

    bool GameController::is_button_down(int controller_id, int button) const {
        return _impl->is_button_down(controller_id, button);
    }

    float GameController::get_axis_value(int controller_id, int axis) const {
        return _impl->get_axis_value(controller_id, axis);
    }

    void GameController::add_listener(IControllerListener &listener) {
        _impl->add_listener(listener);
    }

    bool GameController::remove_listener(const IControllerListener &listener) {
        return _impl->remove_listener(listener);
    }

    void GameController::process_controller_connection_event(int controller_id, bool connected) {
        _impl->process_controller_connection_event(controller_id, connected);
    }

    void GameController::process_controller_button_event(int controller_id, int button, bool down) {
        _impl->process_controller_button_event(controller_id, button, down);
    }

    void GameController::process_controller_axis_event(int controller_id, int axis, float value) {
        _impl->process_controller_axis_event(controller_id, axis, value);
    }

    Input::Input()
        : _keyboard(std::make_unique<Keyboard>())
          , _mouse(std::make_unique<Mouse>())
          , _controller(std::make_unique<GameController>()) {
        _controller->refresh_controllers();
    }

    Input::~Input() = default;

    Keyboard &Input::get_keyboard() {
        return *_keyboard;
    }

    const Keyboard &Input::get_keyboard() const {
        return *_keyboard;
    }

    Mouse &Input::get_mouse() {
        return *_mouse;
    }

    const Mouse &Input::get_mouse() const {
        return *_mouse;
    }

    GameController &Input::get_controller() {
        return *_controller;
    }

    const GameController &Input::get_controller() const {
        return *_controller;
    }

    void Input::process_event(const SDL_Event &event) {
        switch (event.type) {
            // Keyboard events
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                KeyboardKey key = static_cast<KeyboardKey>(event.key.scancode);
                KeyboardModifiers modifiers(event.key.mod);
                bool down = event.key.down;
                _keyboard->process_key_event(key, modifiers, down);
                break;
            }

            case SDL_EVENT_TEXT_INPUT: {
                if (event.text.text[0] != 0) {
                    const UtfChar chr = static_cast<UtfChar>(event.text.text[0]);
                    _keyboard->process_char_event(chr);
                }
                break;
            }

            // Mouse events
            case SDL_EVENT_MOUSE_MOTION: {
                const glm::vec2 position(
                    event.motion.x,
                    event.motion.y
                );
                _mouse->process_motion_event(position);
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL: {
                const glm::vec2 scroll(
                    event.wheel.x,
                    event.wheel.y
                );
                _mouse->process_scroll_event(scroll);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                const auto button = static_cast<MouseButton>(event.button.button);
                const bool down = event.button.down;
                _mouse->process_button_event(button, down);
                break;
            }

            case SDL_EVENT_GAMEPAD_ADDED:
            case SDL_EVENT_GAMEPAD_REMOVED: {
                const int controller_id = event.gdevice.which;
                const bool connected = event.type == SDL_EVENT_GAMEPAD_ADDED;
                _controller->process_controller_connection_event(controller_id, connected);
                break;
            }

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                const int controller_id = event.gbutton.which;
                const int button = event.gbutton.button;
                const bool down = event.gbutton.down;
                _controller->process_controller_button_event(controller_id, button, down);
                break;
            }

            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                int controller_id = event.gaxis.which;
                int axis = event.gaxis.axis;
                float value = static_cast<float>(event.gaxis.value) / 32767.0f;
                _controller->process_controller_axis_event(controller_id, axis, value);
                break;
            }
            default:
                break;
        }
    }
}
