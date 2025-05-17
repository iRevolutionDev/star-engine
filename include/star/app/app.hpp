#pragma once

#include "star/export.hpp"
#include "star/app/app_fwd.hpp"
#include "star/app/app_component.hpp"

#include "star/core/math.hpp"

#include "bgfx/bgfx.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace star {
    struct STAR_EXPORT AppUpdateConfig {
        float fixed_time_step{1.0f / 60.0f};
        float max_frame_time{0.1f};
        bool use_fixed_time_step{false};
    };

    class STAR_EXPORT IAppDelegate {
    public:
        virtual ~IAppDelegate() = default;

        virtual void init() {
        }

        virtual void shutdown() {
        }

        virtual void update(float delta_time) {
        }

        virtual void pre_render() {
        }

        virtual void post_render() {
        }
    };

    class STAR_EXPORT IAppDelegateFactory {
    public:
        virtual ~IAppDelegateFactory() = default;

        virtual std::unique_ptr<IAppDelegate> create_delegate(App &app) = 0;
    };

    class STAR_EXPORT IAppUpdater {
    public:
        virtual ~IAppUpdater() = default;

        virtual void update(float delta_time) = 0;
    };

    template<typename T>
    class STAR_EXPORT ITypeAppUpdater : public IAppUpdater {
    public:
        static constexpr auto type_name() { return typeid(T).name(); }
    };

    class STAR_EXPORT App {
    public:
        App();

        ~App();

        int32_t run(const CmdArgs &args) const;

        void request_render_reset() const;

        void request_renderer_type(bgfx::RendererType::Enum renderer) const;

        void request_quit() const;

        bool is_running() const;

        void set_debug_flag(uint32_t flag, bool enabled = true) const;

        bool get_debug_flag(uint32_t flag) const;

        void set_paused(bool paused) const;

        bool is_paused() const;

        void set_update_config(const AppUpdateConfig &config) const;

        const AppUpdateConfig &get_update_config() const;

        void set_clear_color(const Vector4 &color);

        const Vector4 &get_clear_color() const;

        template<typename T, typename... Args>
        T &add_component(Args &&... args) {
            static_assert(std::is_base_of<IAppComponent, T>::value, "T must be derived from IAppComponent");
            return *static_cast<T *>(add_component_impl(std::make_unique<T>(std::forward<Args>(args)...)).get());
        }

        template<typename T>
        T &get_or_add_component() {
            static_assert(std::is_base_of<IAppComponent, T>::value, "T must be derived from IAppComponent");
            auto component = get_component<T>();
            if (component) {
                return *component;
            }
            return add_component<T>();
        }

        template<typename T>
        T *get_component() {
            return static_cast<T *>(get_component_impl(typeid(T).hash_code()));
        }

        template<typename T>
        bool remove_component() {
            return remove_component_impl(typeid(T).hash_code());
        }

        Window &get_window();

        const Window &get_window() const;

        Input &get_input();

        const Input &get_input() const;

        AssetContext &get_assets();

        const AssetContext &get_assets() const;

        void add_updater(std::unique_ptr<IAppUpdater> &&updater) const;

        void add_updater(IAppUpdater &updater) const;

        template<typename T>
        bool remove_updater() {
            return remove_updater_impl(typeid(T).hash_code());
        }

    private:
        std::shared_ptr<IAppComponent> add_component_impl(std::unique_ptr<IAppComponent> &&component) const;

        IAppComponent *get_component_impl(size_t type_hash) const;

        bool remove_component_impl(size_t type_hash) const;

        bool remove_updater_impl(size_t type_hash) const;

        std::unique_ptr<AppImpl> _impl;
    };

#if defined(_WIN32) && !defined(STAR_CONSOLE_APP)
#define STAR_RUN_APP(AppDelegateClass) \
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { \
        class AppFactory final : public star::IAppDelegateFactory { \
        public: \
            std::unique_ptr<star::IAppDelegate> create_delegate(star::App& app) override { \
                return std::make_unique<AppDelegateClass>(app); \
            } \
        }; \
        return star::main(__argc, const_cast<const char**>(__argv), std::make_unique<AppFactory>()); \
    }
#else
#define STAR_RUN_APP(AppDelegateClass) \
    int main(int argc, char* argv[]) { \
        class AppFactory final : public star::IAppDelegateFactory { \
        public: \
            std::unique_ptr<star::IAppDelegate> create_delegate(star::App& app) override { \
                return std::make_unique<AppDelegateClass>(app); \
            } \
        }; \
        return star::main(argc, const_cast<const char**>(argv), std::make_unique<AppFactory>()); \
    }
#endif
}
