#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "plane.h"

class Frustum {
  public:
    void setMatrix(glm::mat4 matrix);
    Plane planes[6];
    bool sphereInFrustum(glm::vec3 position, float radius);
};
