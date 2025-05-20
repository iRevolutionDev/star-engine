#pragma once

#include "star/export.hpp"
#include "star/app/app_fwd.hpp"
#include <string>
#include <typeinfo>

#include "bgfx/bgfx.h"

namespace star {
    class STAR_EXPORT IAppComponent {
    public:
        virtual ~IAppComponent() = default;

        virtual void init(App &app) {
        }

        virtual void render() {
        }

        virtual void shutdown() {
        }

        virtual void update(float delta_time) {
        }

        virtual size_t get_type_hash() const { return 0; }
        virtual std::string get_type_name() const { return "IAppComponent"; }

        virtual bgfx::ViewId render_reset(const bgfx::ViewId viewId) { return viewId; }
    };

    template<typename T>
    class STAR_EXPORT ITypeAppComponent : public IAppComponent {
    public:
        size_t get_type_hash() const override {
            return typeid(T).hash_code();
        }

        std::string get_type_name() const override {
            return typeid(T).name();
        }

        static constexpr auto type_name() {
            return typeid(T).name();
        }
    };
}
