#pragma once

#include "star/export.hpp"
#include "star/scene/scene.hpp"
#include "star/render/forward_renderer.hpp"
#include <memory>

namespace star {
    class STAR_EXPORT SceneRendererComponent final : public ITypeSceneComponent<SceneRendererComponent> {
    public:
        SceneRendererComponent();

        ~SceneRendererComponent() override;

        void init(Scene &scene, App &app) override;

        void shutdown() override;

        void update(float delta_time) override;

        bgfx::ViewId render_reset(bgfx::ViewId view_id) override;

        ForwardRenderer &get_renderer();

        const ForwardRenderer &get_renderer() const;

    private:
        std::unique_ptr<ForwardRenderer> _renderer;
        Scene *_scene{nullptr};
        App *_app{nullptr};
    };
}
