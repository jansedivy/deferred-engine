#pragma once

#include <glm/vec3.hpp>

enum LightType {
  kPoint,
  kDirectional
};

class Light {
  public:
    glm::vec3 color;
    glm::vec3 direction;
    glm::vec3 position;
    float radius;
    LightType type;
};
