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
    glm::mat4 viewMatrix;
    glm::vec3 right;
    glm::vec3 forward;

    Frustum frustum;

    float fov;
    float aspectRatio;
    float near;
    float far;

    glm::vec3 fogColor;
    float fogDensity;

    void updateMatrix();
    void calculateRightMovement();
    void setAspect(float aspect);
};
