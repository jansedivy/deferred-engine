#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "frustum.h"

class Camera {
  public:
    Camera();
    glm::vec3 position;
    glm::vec3 rotation;
    glm::mat4 view_matrix;
    glm::vec3 right;
    glm::vec3 forward;

    Frustum frustum;

    float fov;
    float aspect_ratio;
    float near;
    float far;

    glm::vec3 fog_color;
    float fog_density;

    void update_matrix();
    void calculate_right_movement();
    void set_aspect(float aspect);
};
