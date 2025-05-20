#include "star/app/window.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace star {
    Window::WindowImpl::WindowImpl() {
    }

    Window::WindowImpl::~WindowImpl() {
        shutdown();
    }

    bool Window::WindowImpl::init(const VideoMode &mode) {
        if (_window) {
            return true;
        }

        _video_mode = mode;

        auto flags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;

        switch (mode.mode) {
            case WindowMode::Fullscreen:
                flags |= SDL_WINDOW_FULLSCREEN;
                break;
            case WindowMode::Borderless:
                flags |= SDL_WINDOW_BORDERLESS;
                break;
            case WindowMode::Windowed:
                break;
        }

        _window = SDL_CreateWindow(
            _title.c_str(),
            static_cast<int>(mode.size.x),
            static_cast<int>(mode.size.y),
            flags
        );

        if (!_window) {
            spdlog::error("Failed to create SDL window: {}", SDL_GetError());
            return false;
        }

        if (mode.display_index >= 0) {
            if (!SDL_SetWindowFullscreenMode(_window, nullptr)) {
                spdlog::warn("Could not set window display: {}", SDL_GetError());
            }
        }

        spdlog::info("Window created: {}x{}", mode.size.x, mode.size.y);
        return true;
    }

    void Window::WindowImpl::shutdown() {
        if (_window) {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }
    }

    bool Window::WindowImpl::is_open() const {
        return _window != nullptr;
    }

    bool Window::WindowImpl::is_focused() const {
        return _is_focused;
    }

    void Window::WindowImpl::set_title(const std::string &title) {
        _title = title;
        if (_window) {
            SDL_SetWindowTitle(_window, title.c_str());
        }
    }

    std::string Window::WindowImpl::get_title() const {
        return _title;
    }

    void Window::WindowImpl::set_size(const glm::uvec2 &size) {
        if (_window) {
            SDL_SetWindowSize(_window, size.x, size.y);
            _video_mode.size = size;
        }
    }

    glm::uvec2 Window::WindowImpl::get_size() const {
        if (!_window) return _video_mode.size;

        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        return glm::uvec2(w, h);
    }

    void Window::WindowImpl::set_position(const glm::ivec2 &position) {
        if (!_window) return;

        SDL_SetWindowPosition(_window, position.x, position.y);
    }

    glm::ivec2 Window::WindowImpl::get_position() const {
        if (!_window) return glm::ivec2(0, 0);

        int x, y;
        SDL_GetWindowPosition(_window, &x, &y);
        return glm::ivec2(x, y);
    }

    void Window::WindowImpl::set_video_mode(const VideoMode &mode) {
        if (!_window) {
            return;
        }

        if (mode.mode == WindowMode::Fullscreen) {
            SDL_SetWindowFullscreen(_window, true);
        } else {
            SDL_SetWindowFullscreen(_window, false);

            if (mode.mode == WindowMode::Borderless) {
                SDL_SetWindowBordered(_window, false);
            } else {
                SDL_SetWindowBordered(_window, true);
            }
        }

        SDL_SetWindowSize(_window, mode.size.x, mode.size.y);

        if (mode.display_index != _video_mode.display_index) {
            if (!SDL_SetWindowFullscreenMode(_window, nullptr)) {
                spdlog::warn("Could not set window display: {}", SDL_GetError());
            }
        }

        _video_mode = mode;
    }

    const VideoMode &Window::WindowImpl::get_video_mode() const {
        return _video_mode;
    }

    glm::vec2 Window::WindowImpl::get_content_scale() {
        return glm::vec2(1.0f);
    }

    SDL_Window *Window::WindowImpl::get_native_handle() const {
        return _window;
    }

    void *Window::WindowImpl::get_native_display() const {
        return nullptr;
    }

    void Window::WindowImpl::process_events() {
        if (!_window) return;
        _is_focused = (SDL_GetWindowFlags(_window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    Window::Window() : _impl(std::make_unique<WindowImpl>()) {
    }

    Window::~Window() = default;

    bool Window::init(const VideoMode &mode) const {
        return _impl->init(mode);
    }

    void Window::shutdown() const {
        _impl->shutdown();
    }

    bool Window::is_open() const {
        return _impl->is_open();
    }

    bool Window::is_focused() const {
        return _impl->is_focused();
    }

    void Window::set_title(const std::string &title) const {
        _impl->set_title(title);
    }

    std::string Window::get_title() const {
        return _impl->get_title();
    }

    void Window::set_size(const glm::uvec2 &size) const {
        _impl->set_size(size);
    }

    glm::uvec2 Window::get_size() const {
        return _impl->get_size();
    }

    void Window::set_position(const glm::ivec2 &position) const {
        _impl->set_position(position);
    }

    glm::ivec2 Window::get_position() const {
        return _impl->get_position();
    }

    void Window::set_video_mode(const VideoMode &mode) const {
        _impl->set_video_mode(mode);
    }

    const VideoMode &Window::get_video_mode() const {
        return _impl->get_video_mode();
    }

    glm::vec2 Window::get_content_scale() const {
        return _impl->get_content_scale();
    }

    SDL_Window *Window::get_native_handle() const {
        return _impl->get_native_handle();
    }

    void *Window::get_native_display() const {
        return _impl->get_native_display();
    }

    void Window::process_events() const {
        _impl->process_events();
    }
}
