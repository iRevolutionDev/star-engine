#pragma once

#include "star/export.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace star {
    class STAR_EXPORT Transform {
    public:
        Transform();

        explicit Transform(const glm::vec3 &position);

        Transform(const glm::vec3 &position, const glm::quat &rotation);

        Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale);

        Transform &set_position(const glm::vec3 &position);

        const glm::vec3 &get_position() const;

        Transform &set_rotation(const glm::quat &rotation);

        const glm::quat &get_rotation() const;

        Transform &set_euler_angles(const glm::vec3 &euler_angles);

        glm::vec3 get_euler_angles() const;

        Transform &set_scale(const glm::vec3 &scale);

        const glm::vec3 &get_scale() const;

        Transform &set_uniform_scale(float scale);

        glm::vec3 get_forward() const;

        glm::vec3 get_right() const;

        glm::vec3 get_up() const;

        glm::mat4 get_model_matrix() const;

        glm::mat4 get_normal_matrix() const;

        Transform &look_at(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 transform_point(const glm::vec3 &point) const;

        glm::vec3 transform_vector(const glm::vec3 &vector) const;

        glm::vec3 inverse_transform_point(const glm::vec3 &point) const;

        glm::vec3 inverse_transform_vector(const glm::vec3 &vector) const;

    private:
        glm::vec3 _position{0.0f};
        glm::quat _rotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 _scale{1.0f};

        mutable bool _matrix_dirty{true};
        mutable glm::mat4 _model_matrix{1.0f};

        void mark_dirty() const;

        void update_matrices() const;
    };
}
