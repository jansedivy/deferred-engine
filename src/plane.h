#pragma once

#include <glm/vec3.hpp>

class Plane {
  public:
    void normalize() {
      float scale = 1.0f / glm::length(normal);
      normal = normal * scale;
      distance *= scale;
    }

    float distanceTo(glm::vec3 position) {
      return glm::dot(normal, position) + distance;
    }

  public:
    glm::vec3 normal;
    float distance;
};
