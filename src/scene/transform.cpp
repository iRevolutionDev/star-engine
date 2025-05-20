#include "star/scene/transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace star {
    Transform::Transform() {
    }

    Transform::Transform(const glm::vec3 &position)
        : _position(position) {
    }

    Transform::Transform(const glm::vec3 &position, const glm::quat &rotation)
        : _position(position)
          , _rotation(rotation) {
    }

    Transform::Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
        : _position(position)
          , _rotation(rotation)
          , _scale(scale) {
    }

    Transform &Transform::set_position(const glm::vec3 &position) {
        _position = position;
        mark_dirty();
        return *this;
    }

    const glm::vec3 &Transform::get_position() const {
        return _position;
    }

    Transform &Transform::set_rotation(const glm::quat &rotation) {
        _rotation = rotation;
        mark_dirty();
        return *this;
    }

    const glm::quat &Transform::get_rotation() const {
        return _rotation;
    }

    Transform &Transform::set_euler_angles(const glm::vec3 &euler_angles) {
        _rotation = glm::quat(glm::radians(euler_angles));
        mark_dirty();
        return *this;
    }

    glm::vec3 Transform::get_euler_angles() const {
        return glm::degrees(glm::eulerAngles(_rotation));
    }

    Transform &Transform::set_scale(const glm::vec3 &scale) {
        _scale = scale;
        mark_dirty();
        return *this;
    }

    const glm::vec3 &Transform::get_scale() const {
        return _scale;
    }

    Transform &Transform::set_uniform_scale(float scale) {
        _scale = glm::vec3(scale);
        mark_dirty();
        return *this;
    }

    glm::vec3 Transform::get_forward() const {
        return glm::rotate(_rotation, glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 Transform::get_right() const {
        return glm::rotate(_rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 Transform::get_up() const {
        return glm::rotate(_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 Transform::get_model_matrix() const {
        if (_matrix_dirty) {
            update_matrices();
        }
        return _model_matrix;
    }

    glm::mat4 Transform::get_normal_matrix() const {
        if (_matrix_dirty) {
            update_matrices();
        }

        glm::mat3 normal_matrix = glm::mat3(_model_matrix);

        if (_scale.x != _scale.y || _scale.x != _scale.z || _scale.y != _scale.z) {
            normal_matrix = glm::transpose(glm::inverse(normal_matrix));
        }

        return glm::mat4(normal_matrix);
    }

    Transform &Transform::look_at(const glm::vec3 &target, const glm::vec3 &up) {
        glm::vec3 direction = glm::normalize(target - _position);

        if (glm::length(direction) < 0.001f) {
            return *this;
        }

        const glm::mat4 look_matrix = glm::lookAt(_position, target, up);

        _rotation = glm::quat_cast(look_matrix);

        mark_dirty();
        return *this;
    }

    glm::vec3 Transform::transform_point(const glm::vec3 &point) const {
        return glm::vec3(get_model_matrix() * glm::vec4(point, 1.0f));
    }

    glm::vec3 Transform::transform_vector(const glm::vec3 &vector) const {
        return glm::vec3(glm::mat3(get_model_matrix()) * vector);
    }

    glm::vec3 Transform::inverse_transform_point(const glm::vec3 &point) const {
        glm::vec3 scaled_point = point - _position;
        glm::quat inv_rotation = glm::inverse(_rotation);
        scaled_point = glm::rotate(inv_rotation, scaled_point);

        glm::vec3 inv_scale;
        inv_scale.x = (_scale.x != 0.0f) ? 1.0f / _scale.x : 0.0f;
        inv_scale.y = (_scale.y != 0.0f) ? 1.0f / _scale.y : 0.0f;
        inv_scale.z = (_scale.z != 0.0f) ? 1.0f / _scale.z : 0.0f;

        return scaled_point * inv_scale;
    }

    glm::vec3 Transform::inverse_transform_vector(const glm::vec3 &vector) const {
        glm::quat inv_rotation = glm::inverse(_rotation);
        glm::vec3 rotated_vector = glm::rotate(inv_rotation, vector);

        glm::vec3 inv_scale;
        inv_scale.x = (_scale.x != 0.0f) ? 1.0f / _scale.x : 0.0f;
        inv_scale.y = (_scale.y != 0.0f) ? 1.0f / _scale.y : 0.0f;
        inv_scale.z = (_scale.z != 0.0f) ? 1.0f / _scale.z : 0.0f;

        return rotated_vector * inv_scale;
    }

    void Transform::mark_dirty() const {
        _matrix_dirty = true;
    }

    void Transform::update_matrices() const {
        _model_matrix = glm::translate(glm::mat4(1.0f), _position);
        _model_matrix = _model_matrix * glm::mat4_cast(_rotation);
        _model_matrix = glm::scale(_model_matrix, _scale);

        _matrix_dirty = false;
    }
}
