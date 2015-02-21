#include "aabb.h"

void AABB::calculateForMesh(std::vector<float> vertices) {
  bool first = true;
  for (auto it = vertices.begin(); it != vertices.end(); it += 3) {
    float x = *it;
    float y = *(it+1);
    float z = *(it+2);

    if (first) {
      minX = x;
      minY = y;
      maxX = x;
      maxY = y;
      maxZ = z;
      maxZ = z;
      first = false;
    } else {
      if (x < minX) { minX = x; }
      if (x > maxX) { maxX = x; }

      if (y < minY) { minY = y; }
      if (y > maxY) { maxY = y; }

      if (z < minZ) { minZ = z; }
      if (z > maxZ) { maxZ = z; }
    }
  }
}

AABB AABB::translate(glm::vec3 value) {
  AABB result;

  return result;
}
