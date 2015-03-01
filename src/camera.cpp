#include "camera.h"
#include <stdio.h>

Camera::Camera() {
  aspectRatio = 4.0f / 3.0f;
  fov = 70.0f;
  near = 0.1f;
  far = 10000.f;

  fogColor = glm::vec3(0.2, 0.2, 0.4);
  fogDensity = -3.7;
}

void Camera::setAspect(float aspect) {
  aspectRatio = aspect;
}

void Camera::updateMatrix() {
  viewMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);

  viewMatrix = glm::rotate(viewMatrix, rotation.x, glm::vec3(1.0, 0.0, 0.0));
  viewMatrix = glm::rotate(viewMatrix, rotation.y, glm::vec3(0.0, 1.0, 0.0));
  viewMatrix = glm::rotate(viewMatrix, rotation.z, glm::vec3(0.0, 0.0, 1.0));

  viewMatrix = glm::translate(viewMatrix, (position * -1.0f));

  frustum.setMatrix(viewMatrix);
}

void Camera::calculateRightMovement() {
  forward[0] = sin(rotation[1]) * cos(rotation[0]);
  forward[1] = -sin(rotation[0]);
  forward[2] = -cos(rotation[1]) * cos(rotation[0]);

  right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0)));
}
