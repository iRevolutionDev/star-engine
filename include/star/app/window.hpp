#pragma once

#include "star/export.hpp"
#include <string>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

namespace star {
    enum class WindowMode {
        Windowed,
        Borderless,
        Fullscreen
    };

    struct STAR_EXPORT VideoMode {
        glm::uvec2 size{1280, 720};
        int display_index{0};
        WindowMode mode{WindowMode::Windowed};
        bool vsync{true};

        bool operator==(const VideoMode &other) const {
            return size == other.size &&
                   display_index == other.display_index &&
                   mode == other.mode &&
                   vsync == other.vsync;
        }

        bool operator!=(const VideoMode &other) const {
            return !(*this == other);
        }
    };

    class STAR_EXPORT Window {
    public:
        Window();

        ~Window();

        bool init(const VideoMode &mode) const;

        void shutdown() const;

        bool is_open() const;

        bool is_focused() const;

        void set_title(const std::string &title) const;

        std::string get_title() const;

        void set_size(const glm::uvec2 &size) const;

        glm::uvec2 get_size() const;

        void set_position(const glm::ivec2 &position) const;

        glm::ivec2 get_position() const;

        void set_video_mode(const VideoMode &mode) const;

        const VideoMode &get_video_mode() const;

        glm::vec2 get_content_scale() const;

        SDL_Window *get_native_handle() const;

        void process_events() const;

        void *get_native_display() const;

    private:
        class WindowImpl;
        std::unique_ptr<WindowImpl> _impl;
    };
}
