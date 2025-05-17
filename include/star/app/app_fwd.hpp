#pragma once

#include "star/export.hpp"
#include <cstdint>
#include <memory>
#include <span>

namespace star {
    class App;
    class AppImpl;
    class Input;
    class Window;
    class AssetContext;
    class RenderChain;

    class IAppComponent;
    class IAppDelegate;
    class IAppDelegateFactory;
    class IAppUpdater;

    using CmdArgs = std::span<const char *>;

    STAR_EXPORT int32_t main(int32_t argc, const char *argv[], std::unique_ptr<IAppDelegateFactory> &&factory);
}
