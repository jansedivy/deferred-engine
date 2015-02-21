#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
  public:
    Camera();
    glm::vec3 position;
    glm::quat rotation;
    glm::mat4 viewMatrix;
    glm::vec3 right;
    glm::vec3 forward;

    float fov;
    float aspectRatio;
    float near;
    float far;

    void updateMatrix();
    void calculateRightMovement();
    void setAspect(float aspect);
};
