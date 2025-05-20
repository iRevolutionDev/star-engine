#include "star/core/common.hpp"

#include "star/app/app.hpp"
#include "star/app/window.hpp"
#include "star/app/input.hpp"

#include <spdlog/spdlog.h>

namespace star {
    int32_t main(const int32_t argc, const char *argv[], std::unique_ptr<IAppDelegateFactory> &&factory) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD) == 0) {
            spdlog::error("SDL initialization failed: {}", SDL_GetError());
            return 1;
        }

        const auto app = std::make_unique<App>();

        if (factory) {
            app->set_delegate(factory->create_delegate(*app));
        }

        const CmdArgs args(argv, argc);

        const auto result = app->run(args);

        SDL_Quit();
        return result;
    }

    AppImpl::AppImpl(App &app)
        : _app(app)
          , _window(std::make_unique<Window>())
          , _input(std::make_unique<Input>()) {
        _input->get_keyboard().add_listener(*this);
    }

    AppImpl::~AppImpl() {
        shutdown();
    }

    int32_t AppImpl::run(const CmdArgs &args) {
        if (!initialize(args)) {
            return 1;
        }

        _running = true;

        while (_running) {
            const float delta_time = update_time_passed();

            process_events();

            if (!_paused) {
                update_frame(delta_time);
            }

            render_frame();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        shutdown();
        return 0;
    }

    bool AppImpl::initialize(const CmdArgs &args) {
        if (!_window->init(_video_mode)) {
            spdlog::error("Failed to initialize window");
            return false;
        }

        bgfx_init();

        for (const auto &component: _components) {
            component->init(_app);
        }

        if (_delegate) {
            _delegate->init();
        }

        return true;
    }

    void AppImpl::shutdown() {
        if (!_running) {
            return;
        }

        _running = false;

        if (_delegate) {
            _delegate->shutdown();
        }

        for (auto it = _components.rbegin(); it != _components.rend(); ++it) {
            (*it)->shutdown();
        }

        bgfx::shutdown();

        _window->shutdown();
    }

    void AppImpl::process_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                _running = false;
            }

            _input->process_event(event);
        }
    }

    void AppImpl::update_frame(const float delta_time) {
        if (_delegate) {
            _delegate->update(delta_time);
        }

        for (const auto &component: Components(_components)) {
            component->update(delta_time);
        }

        for (auto &updater: _updater_refs) {
            updater.get().update(delta_time);
        }

        const auto &size = _window->get_size();
        const auto &video_mode = _window->get_video_mode();
        if (_render_size != size || _video_mode != video_mode || _active_reset_flags != _reset_flags) {
            _render_size = size;
            _video_mode = video_mode;
            _active_reset_flags = _reset_flags;
            request_render_reset();
        }

        if (_render_reset) {
            _render_reset = false;
            render_reset();
        }
    }

    void AppImpl::render_frame() {
        if (_delegate) {
            _delegate->pre_render();
        }

        bgfx::touch(0);
        bgfx::dbgTextClear();

        if (_delegate) {
            _delegate->render();
        }

        for (const auto &component: Components(_components)) {
            component->render();
        }

        if (_delegate) {
            _delegate->post_render();
        }

        bgfx::frame();
    }

    void AppImpl::render_reset() {
        const auto size = _window->get_size();
        bgfx::reset(size.x, size.y, _active_reset_flags);

        const auto numViews = bgfx::getCaps()->limits.maxViews;
        for (bgfx::ViewId i = 0; i < numViews; ++i) {
            bgfx::resetView(i);
        }

        bgfx::ViewId id = 0;

        bgfx::setViewName(id, "App clear");
        bgfx::setViewRect(id, 0, 0, bgfx::BackbufferRatio::Equal);
        constexpr uint16_t clearFlags = BGFX_CLEAR_DEPTH | BGFX_CLEAR_COLOR | BGFX_CLEAR_STENCIL;
        static constexpr uint8_t clearColor = 1;
        bgfx::setViewClear(id, clearFlags, 1.F, 0U,
                           clearColor, clearColor, clearColor, clearColor,
                           clearColor, clearColor, clearColor, clearColor);
        ++id;

        if (_delegate) {
            id = _delegate->render_reset(id);
        }

        for (const auto &component: Components(_components)) {
            id = component->render_reset(id);
        }
    }

    void AppImpl::bgfx_init() const {
        bgfx::Init init;
        bgfx::PlatformData platform_data{};

        if (_renderer_type != bgfx::RendererType::Count) {
            init.type = _renderer_type;
        }

        SDL_Window *window = _window->get_native_handle();

#if defined(STAR_PLATFORM_WINDOWS)
        const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                                                   SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                                                   nullptr));
        platform_data.nwh = hwnd;
#elif defined(STAR_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        Display *xdisplay = (Display *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (xdisplay && xwindow) {
            platform_data.ndt = xdisplay;
            platform_data.nwh = (void *)(uintptr_t)xwindow;
        }
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        struct wl_display *display = (struct wl_display *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        struct wl_surface *surface = (struct wl_surface *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        if (display && surface) {
            platform_data.ndt = xdisplay;
            platform_data.nwh = (void *)(uintptr_t)xwindow;
        }
    }
#endif

        init.platformData = platform_data;

        if (!bgfx::init(init)) {
            spdlog::error("Failed to initialize bgfx");
            return;
        }

        bgfx::setDebug(_debug_flags);

        bgfx::setViewClear(0,
                           BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                           static_cast<uint32_t>(_clear_color.r * 255) << 24 |
                           static_cast<uint32_t>(_clear_color.g * 255) << 16 |
                           static_cast<uint32_t>(_clear_color.b * 255) << 8 |
                           static_cast<uint32_t>(_clear_color.a * 255),
                           1.0f, 0);

        const auto size = _window->get_size();
        bgfx::setViewRect(0, 0, 0, size.x, size.y);
    }

    float AppImpl::update_time_passed() const {
        const auto now = bx::getHPCounter();

        if (_last_update == 0) {
            return 0.0f;
        }

        auto delta_time = static_cast<float>(static_cast<double>(now - _last_update) / static_cast<double>(
                                                 bx::getHPFrequency()));

        delta_time = std::min(delta_time, _update_config.max_frame_time);

        return delta_time;
    }

    void AppImpl::request_render_reset() {
        _render_reset = true;
    }

    void AppImpl::request_renderer_type(bgfx::RendererType::Enum renderer) {
        if (_renderer_type != renderer) {
            _renderer_type = renderer;
            if (_running) {
                spdlog::info("Renderer type change requires application restart");
            }
        }
    }

    void AppImpl::request_quit() {
        _running = false;
    }

    bool AppImpl::is_running() const {
        return _running;
    }

    void AppImpl::set_debug_flag(const uint32_t flag, const bool enabled) {
        if (enabled) {
            _debug_flags |= flag;
        } else {
            _debug_flags &= ~flag;
        }

        if (_running) {
            bgfx::setDebug(_debug_flags);
        }
    }

    bool AppImpl::get_debug_flag(uint32_t flag) const {
        return (_debug_flags & flag) != 0;
    }

    void AppImpl::set_paused(bool paused) {
        _paused = paused;
    }

    bool AppImpl::is_paused() const {
        return _paused;
    }

    void AppImpl::set_update_config(const AppUpdateConfig &config) {
        _update_config = config;
    }

    const AppUpdateConfig &AppImpl::get_update_config() const {
        return _update_config;
    }

    void AppImpl::set_clear_color(const glm::vec4 &color) {
        _clear_color = color;
    }

    const glm::vec4 &AppImpl::get_clear_color() const {
        return _clear_color;
    }

    void AppImpl::set_delegate(std::unique_ptr<IAppDelegate> delegate) {
        _delegate = std::move(delegate);
    }

    void AppImpl::add_component(std::shared_ptr<IAppComponent> &&component) {
        if (const auto type_hash = component->get_type_hash()) {
            remove_component(type_hash);
        }

        if (_running) {
            component->init(_app);
        }

        _components.push_back(std::move(component));
    }

    bool AppImpl::remove_component(size_t type_hash) {
        const auto it = std::ranges::find_if(_components,
                                             [type_hash](const auto &comp) {
                                                 return comp->get_type_hash() == type_hash;
                                             });

        if (it != _components.end()) {
            if (_running) {
                (*it)->shutdown();
            }
            _components.erase(it);
            return true;
        }

        return false;
    }

    bool AppImpl::has_component(size_t type_hash) const {
        return std::ranges::any_of(_components,
                                   [type_hash](const auto &comp) {
                                       return comp->get_type_hash() == type_hash;
                                   });
    }

    IAppComponent *AppImpl::get_component(size_t type_hash) {
        const auto it = std::ranges::find_if(_components,
                                             [type_hash](const auto &comp) {
                                                 return comp->get_type_hash() == type_hash;
                                             });

        return it != _components.end() ? it->get() : nullptr;
    }

    const IAppComponent *AppImpl::get_component(size_t type_hash) const {
        const auto it = std::ranges::find_if(_components,
                                             [type_hash](const auto &comp) {
                                                 return comp->get_type_hash() == type_hash;
                                             });

        return (it != _components.end()) ? it->get() : nullptr;
    }

    Input &AppImpl::get_input() {
        return *_input;
    }

    const Input &AppImpl::get_input() const {
        return *_input;
    }

    Window &AppImpl::get_window() {
        return *_window;
    }

    const Window &AppImpl::get_window() const {
        return *_window;
    }

    // AssetContext& AppImpl::get_assets() {
    // }
    //
    // const AssetContext& AppImpl::get_assets() const {
    // }

    void AppImpl::add_updater(std::unique_ptr<IAppUpdater> &&updater) {
        _updaters.push_back(std::move(updater));
    }

    void AppImpl::add_updater(IAppUpdater &updater) {
        _updater_refs.push_back(std::ref(updater));
    }

    bool AppImpl::remove_updater(size_t type_hash) {
        return false;
    }

    void AppImpl::on_keyboard_key(const KeyboardKey key, const KeyboardModifiers &modifiers, bool down) {
        if (down) {
            handle_debug_shortcuts(key, modifiers);
        }
    }

    void AppImpl::handle_debug_shortcuts(KeyboardKey key, const KeyboardModifiers &modifiers) {
        if (key == KeyboardKey::F11) {
            VideoMode mode = _window->get_video_mode();
            if (mode.mode == WindowMode::Fullscreen) {
                mode.mode = WindowMode::Windowed;
            } else {
                mode.mode = WindowMode::Fullscreen;
            }
            _window->set_video_mode(mode);
        }

        if (key == KeyboardKey::F5) {
            VideoMode mode = _window->get_video_mode();
            mode.vsync = !mode.vsync;
            _window->set_video_mode(mode);
        }

        if (key == KeyboardKey::Escape && modifiers.has(KeyboardModifier::Ctrl)) {
            request_quit();
        }
    }

    App::App() : _impl(std::make_unique<AppImpl>(*this)) {
    }

    App::~App() = default;

    int32_t App::run(const CmdArgs &args) const {
        return _impl->run(args);
    }

    void App::request_render_reset() const {
        _impl->request_render_reset();
    }

    void App::request_renderer_type(const bgfx::RendererType::Enum renderer) const {
        _impl->request_renderer_type(renderer);
    }

    void App::request_quit() const {
        _impl->request_quit();
    }

    bool App::is_running() const {
        return _impl->is_running();
    }

    void App::set_debug_flag(const uint32_t flag, const bool enabled) const {
        _impl->set_debug_flag(flag, enabled);
    }

    bool App::get_debug_flag(const uint32_t flag) const {
        return _impl->get_debug_flag(flag);
    }

    void App::set_paused(const bool paused) const {
        _impl->set_paused(paused);
    }

    bool App::is_paused() const {
        return _impl->is_paused();
    }

    void App::set_update_config(const AppUpdateConfig &config) const {
        _impl->set_update_config(config);
    }

    const AppUpdateConfig &App::get_update_config() const {
        return _impl->get_update_config();
    }

    void App::set_clear_color(const Vector4 &color) {
        _impl->set_clear_color(color);
    }

    const Vector4 &App::get_clear_color() const {
        return _impl->get_clear_color();
    }

    void App::set_delegate(std::unique_ptr<IAppDelegate> delegate) const {
        _impl->set_delegate(std::move(delegate));
    }

    std::shared_ptr<IAppComponent> App::add_component_impl(std::unique_ptr<IAppComponent> &&component) const {
        auto shared_component = std::shared_ptr<IAppComponent>(component.release());
        auto component_copy = shared_component;
        _impl->add_component(std::move(shared_component));
        return component_copy;
    }

    IAppComponent *App::get_component_impl(const size_t type_hash) const {
        return _impl->get_component(type_hash);
    }

    bool App::remove_component_impl(const size_t type_hash) const {
        return _impl->remove_component(type_hash);
    }

    Window &App::get_window() {
        return _impl->get_window();
    }

    const Window &App::get_window() const {
        return _impl->get_window();
    }

    Input &App::get_input() {
        return _impl->get_input();
    }

    const Input &App::get_input() const {
        return _impl->get_input();
    }

    // AssetContext& App::get_assets() {
    //     return _impl->get_assets();
    // }
    //
    // const AssetContext& App::get_assets() const {
    // }

    void App::add_updater(std::unique_ptr<IAppUpdater> &&updater) const {
        _impl->add_updater(std::move(updater));
    }

    void App::add_updater(IAppUpdater &updater) const {
        _impl->add_updater(updater);
    }

    bool App::remove_updater_impl(const size_t type_hash) const {
        return _impl->remove_updater(type_hash);
    }
}
