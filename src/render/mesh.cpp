#include "star/render/mesh.hpp"
#include <glm/gtc/constants.hpp>
#include <spdlog/spdlog.h>

namespace star {
    bgfx::VertexLayout Vertex::ms_layout;

    void Vertex::init() {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .end();
    }

    Mesh::Mesh() = default;

    Mesh::~Mesh() {
        destroy();
    }

    Mesh::Mesh(Mesh &&other) noexcept
        : _vbh(other._vbh)
          , _ibh(other._ibh)
          , _vertex_count(other._vertex_count)
          , _index_count(other._index_count) {
        other._vbh = BGFX_INVALID_HANDLE;
        other._ibh = BGFX_INVALID_HANDLE;
        other._vertex_count = 0;
        other._index_count = 0;
    }

    Mesh &Mesh::operator=(Mesh &&other) noexcept {
        if (this != &other) {
            destroy();

            _vbh = other._vbh;
            _ibh = other._ibh;
            _vertex_count = other._vertex_count;
            _index_count = other._index_count;

            other._vbh = BGFX_INVALID_HANDLE;
            other._ibh = BGFX_INVALID_HANDLE;
            other._vertex_count = 0;
            other._index_count = 0;
        }
        return *this;
    }

    bool Mesh::create(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
        destroy();

        if (vertices.empty()) {
            spdlog::error("Cannot create mesh with empty vertex data");
            return false;
        }

        const bgfx::Memory *vbmem = bgfx::copy(vertices.data(),
                                               static_cast<uint32_t>(vertices.size() * sizeof(Vertex)));
        _vbh = bgfx::createVertexBuffer(vbmem, Vertex::ms_layout);

        if (!indices.empty()) {
            const bgfx::Memory *ibmem = bgfx::copy(indices.data(),
                                                   static_cast<uint32_t>(indices.size() * sizeof(uint16_t)));
            _ibh = bgfx::createIndexBuffer(ibmem);
        }

        _vertex_count = static_cast<uint32_t>(vertices.size());
        _index_count = static_cast<uint32_t>(indices.size());

        return is_valid();
    }

    bool Mesh::create(const std::vector<Vertex> &vertices) {
        return create(vertices, {});
    }

    Mesh Mesh::create_cube(float size) {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        float hs = size * 0.5f;

        const glm::vec3 corners[8] = {
            {-hs, -hs, -hs},
            {hs, -hs, -hs},
            {hs, hs, -hs},
            {-hs, hs, -hs},
            {-hs, -hs, hs},
            {hs, -hs, hs},
            {hs, hs, hs},
            {-hs, hs, hs}
        };

        const glm::vec3 normals[6] = {
            {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, 1.0f},
            {-1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, -1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        };

        glm::vec2 texcoords[4] = {
            {0.0f, 1.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f},
            {0.0f, 0.0f}
        };

        constexpr glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        vertices.push_back({corners[0], normals[0], texcoords[0], color}); // 0
        vertices.push_back({corners[1], normals[0], texcoords[1], color}); // 1
        vertices.push_back({corners[2], normals[0], texcoords[2], color}); // 2
        vertices.push_back({corners[3], normals[0], texcoords[3], color}); // 3

        vertices.push_back({corners[4], normals[1], texcoords[0], color}); // 4
        vertices.push_back({corners[5], normals[1], texcoords[1], color}); // 5
        vertices.push_back({corners[6], normals[1], texcoords[2], color}); // 6
        vertices.push_back({corners[7], normals[1], texcoords[3], color}); // 7

        vertices.push_back({corners[0], normals[2], texcoords[0], color}); // 8
        vertices.push_back({corners[3], normals[2], texcoords[1], color}); // 9
        vertices.push_back({corners[7], normals[2], texcoords[2], color}); // 10
        vertices.push_back({corners[4], normals[2], texcoords[3], color}); // 11

        vertices.push_back({corners[1], normals[3], texcoords[0], color}); // 12
        vertices.push_back({corners[5], normals[3], texcoords[1], color}); // 13
        vertices.push_back({corners[6], normals[3], texcoords[2], color}); // 14
        vertices.push_back({corners[2], normals[3], texcoords[3], color}); // 15

        vertices.push_back({corners[0], normals[4], texcoords[0], color}); // 16
        vertices.push_back({corners[4], normals[4], texcoords[1], color}); // 17
        vertices.push_back({corners[5], normals[4], texcoords[2], color}); // 18
        vertices.push_back({corners[1], normals[4], texcoords[3], color}); // 19

        vertices.push_back({corners[3], normals[5], texcoords[0], color}); // 20
        vertices.push_back({corners[2], normals[5], texcoords[1], color}); // 21
        vertices.push_back({corners[6], normals[5], texcoords[2], color}); // 22
        vertices.push_back({corners[7], normals[5], texcoords[3], color}); // 23

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(4);
        indices.push_back(6);
        indices.push_back(7);

        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(8);
        indices.push_back(10);
        indices.push_back(11);

        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(12);
        indices.push_back(14);
        indices.push_back(15);

        indices.push_back(16);
        indices.push_back(17);
        indices.push_back(18);
        indices.push_back(16);
        indices.push_back(18);
        indices.push_back(19);

        indices.push_back(20);
        indices.push_back(21);
        indices.push_back(22);
        indices.push_back(20);
        indices.push_back(22);
        indices.push_back(23);

        Mesh mesh;
        mesh.create(vertices, indices);
        return mesh;
    }

    Mesh Mesh::create_sphere(float radius, uint32_t segments) {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        if (segments < 3) segments = 3;

        const float pi = glm::pi<float>();
        const float two_pi = 2.0f * pi;

        const glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        vertices.push_back({
            glm::vec3(0.0f, radius, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.5f, 0.0f),
            color
        });

        for (uint32_t y = 0; y < segments - 1; ++y) {
            const float phi = pi * (y + 1) / segments;
            const float sin_phi = std::sin(phi);
            const float cos_phi = std::cos(phi);

            for (uint32_t x = 0; x < segments; ++x) {
                const float theta = two_pi * x / segments;
                const float sin_theta = std::sin(theta);
                const float cos_theta = std::cos(theta);

                const float px = radius * sin_phi * cos_theta;
                const float py = radius * cos_phi;
                const float pz = radius * sin_phi * sin_theta;

                const glm::vec3 position(px, py, pz);
                const glm::vec3 normal = glm::normalize(position);

                const float u = static_cast<float>(x) / segments;
                const float v = static_cast<float>(y + 1) / segments;

                vertices.push_back({
                    position,
                    normal,
                    glm::vec2(u, v),
                    color
                });
            }
        }

        vertices.push_back({
            glm::vec3(0.0f, -radius, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec2(0.5f, 1.0f),
            color
        });

        for (uint32_t x = 0; x < segments; ++x) {
            const uint16_t next_x = (x + 1) % segments;

            indices.push_back(0);
            indices.push_back(1 + x);
            indices.push_back(1 + next_x);
        }

        for (uint32_t y = 0; y < segments - 2; ++y) {
            const uint16_t ring_start = 1 + y * segments;
            const uint16_t next_ring_start = 1 + (y + 1) * segments;

            for (uint32_t x = 0; x < segments; ++x) {
                const uint16_t next_x = (x + 1) % segments;

                indices.push_back(ring_start + x);
                indices.push_back(next_ring_start + x);
                indices.push_back(ring_start + next_x);

                indices.push_back(ring_start + next_x);
                indices.push_back(next_ring_start + x);
                indices.push_back(next_ring_start + next_x);
            }
        }

        const uint16_t bottom_vertex = static_cast<uint16_t>(vertices.size() - 1);
        const uint16_t last_ring_start = bottom_vertex - segments;

        for (uint32_t x = 0; x < segments; ++x) {
            const uint16_t next_x = (x + 1) % segments;

            indices.push_back(bottom_vertex);
            indices.push_back(last_ring_start + next_x);
            indices.push_back(last_ring_start + x);
        }

        Mesh mesh;
        mesh.create(vertices, indices);
        return mesh;
    }

    Mesh Mesh::create_plane(float width, float height) {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        float hw = width * 0.5f;
        float hh = height * 0.5f;

        glm::vec3 corners[4] = {
            {-hw, 0.0f, -hh},
            {hw, 0.0f, -hh},
            {hw, 0.0f, hh},
            {-hw, 0.0f, hh}
        };

        glm::vec3 normal(0.0f, 1.0f, 0.0f);

        glm::vec2 texcoords[4] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

        for (int i = 0; i < 4; ++i) {
            vertices.push_back({corners[i], normal, texcoords[i], color});
        }

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        Mesh mesh;
        mesh.create(vertices, indices);
        return mesh;
    }

    bool Mesh::draw(bgfx::Encoder *encoder) const {
        if (!is_valid()) return false;

        encoder->setVertexBuffer(0, _vbh);

        if (bgfx::isValid(_ibh)) {
            encoder->setIndexBuffer(_ibh);
        }

        return true;
    }

    bool Mesh::is_valid() const {
        return bgfx::isValid(_vbh);
    }

    uint32_t Mesh::get_vertex_count() const {
        return _vertex_count;
    }

    uint32_t Mesh::get_index_count() const {
        return _index_count;
    }

    void Mesh::destroy() {
        if (bgfx::isValid(_ibh)) {
            bgfx::destroy(_ibh);
            _ibh = BGFX_INVALID_HANDLE;
        }

        if (bgfx::isValid(_vbh)) {
            bgfx::destroy(_vbh);
            _vbh = BGFX_INVALID_HANDLE;
        }

        _vertex_count = 0;
        _index_count = 0;
    }
}
