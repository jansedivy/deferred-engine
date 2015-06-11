#include "camera.h"
#include <stdio.h>

Camera::Camera() {
  aspect_ratio = 4.0f / 3.0f;
  fov = 70.0f;
  near = 0.1f;
  far = 10000.f;

  fog_color = glm::vec3(0.2, 0.2, 0.4);
  fog_density = -3.7;
}

void Camera::set_aspect(float aspect) {
  aspect_ratio = aspect;
}

void Camera::update_matrix() {
  view_matrix = glm::perspective(glm::radians(fov), aspect_ratio, near, far);

  view_matrix = glm::rotate(view_matrix, rotation.x, glm::vec3(1.0, 0.0, 0.0));
  view_matrix = glm::rotate(view_matrix, rotation.y, glm::vec3(0.0, 1.0, 0.0));
  view_matrix = glm::rotate(view_matrix, rotation.z, glm::vec3(0.0, 0.0, 1.0));

  view_matrix = glm::translate(view_matrix, (position * -1.0f));

  frustum.setMatrix(view_matrix);
}

void Camera::calculate_right_movement() {
  forward[0] = sin(rotation[1]) * cos(rotation[0]);
  forward[1] = -sin(rotation[0]);
  forward[2] = -cos(rotation[1]) * cos(rotation[0]);

  right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0)));
}
