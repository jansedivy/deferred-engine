#pragma once

#include <glm/vec3.hpp>

enum PlaneType {
  kLeftPlane = 0,
  kRightPlane = 1,
  kTopPlane = 2,
  kBottomPlane = 3,
  kNearPlane = 4,
  kFarPlane = 5
};

class Plane {
  public:

    void normalize() {
      float mag = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

      normal = normal / mag;

      distance = distance / mag;
    }

    float distanceTo(glm::vec3 position) {
      return normal.x * position.x + normal.y * position.y + normal.z * position.z + distance;
    }

  public:
    glm::vec3 normal;
    float distance;
};
