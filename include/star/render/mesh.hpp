#pragma once

#include "star/export.hpp"
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

namespace star {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        glm::vec4 color;

        static void init();

        static bgfx::VertexLayout ms_layout;
    };

    class STAR_EXPORT IMesh {
    public:
        virtual ~IMesh() = default;

        virtual bool render(bgfx::Encoder &encoder) const = 0;

        virtual const bgfx::VertexLayout &get_vertex_layout() const = 0;

        static std::unique_ptr<IMesh> create(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
    };

    class STAR_EXPORT Mesh {
    public:
        Mesh();

        ~Mesh();

        Mesh(const Mesh &) = delete;

        Mesh &operator=(const Mesh &) = delete;

        Mesh(Mesh &&other) noexcept;

        Mesh &operator=(Mesh &&other) noexcept;

        bool create(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);

        bool create(const std::vector<Vertex> &vertices);

        static Mesh create_cube(float size = 1.0f);

        static Mesh create_sphere(float radius = 1.0f, uint32_t segments = 16);

        static Mesh create_plane(float width = 1.0f, float height = 1.0f);

        void draw(bgfx::Encoder *encoder) const;

        bool is_valid() const;

        uint32_t get_vertex_count() const;

        uint32_t get_index_count() const;

    private:
        void destroy();

        bgfx::VertexBufferHandle _vbh{BGFX_INVALID_HANDLE};
        bgfx::IndexBufferHandle _ibh{BGFX_INVALID_HANDLE};
        uint32_t _vertex_count{0};
        uint32_t _index_count{0};
    };
}
