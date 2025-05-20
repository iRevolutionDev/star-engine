#pragma once

#include "star/export.hpp"
#include "star/render/renderer.hpp"
#include "star/utils/memory/optional_ref.hpp"

namespace star {
    class Mesh;
    class Material;
    class Light;

    class STAR_EXPORT ForwardRenderer final : public Renderer {
    public:
        ForwardRenderer();

        ~ForwardRenderer() override;

        void init(Scene &scene, App &app) override;

        void shutdown() override;

        void render(bgfx::ViewId view_id, bgfx::Encoder *encoder = nullptr) override;

        bgfx::ViewId render_reset(bgfx::ViewId view_id) override;

        RendererType get_renderer_type() const override { return RendererType::Forward; }
        std::string get_renderer_name() const override { return "ForwardRenderer"; }
    };

    class STAR_EXPORT ForwardRendererComponent final : public ITypeCameraComponent<ForwardRendererComponent> {
    public:
        ForwardRendererComponent();

        ~ForwardRendererComponent() override;

        void init(Camera &camera, Scene &scene, App &app) override;

        void shutdown() override;

        void render() override;

        bgfx::ViewId render_reset(bgfx::ViewId view_id) override;

        ForwardRenderer &get_renderer();

        const ForwardRenderer &get_renderer() const;

    private:
        std::unique_ptr<ForwardRenderer> _renderer;
        OptionalRef<Camera> _camera;
        OptionalRef<Scene> _scene;
        OptionalRef<App> _app;
        std::optional<bgfx::ViewId> _view_id;
    };
}
