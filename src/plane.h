#pragma once

#include <glm/vec3.hpp>

class Plane {
  public:
    void normalize() {
      float scale = 1.0f / glm::length(normal);
      normal = normal * scale;
      distance *= scale;
    }

  public:
    glm::vec3 normal;
    float distance;
};
