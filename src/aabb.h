#pragma once

#include <vector>
#include <glm/vec3.hpp>

class AABB {
  public:
    float minX;
    float maxX;

    float minY;
    float maxY;

    float minZ;
    float maxZ;

    void calculateForMesh(std::vector<float> vertices);
    AABB translate(glm::vec3 value);
};
