#pragma once

#include <glm/vec3.hpp>
#include "mesh.h"
#include "texture.h"

enum EntityType {
  kPlanet,
  kAsteroid,
  kBlock,
  kOther
};

class Entity {
  public:
    Mesh *mesh;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 color;
    glm::vec3 scale;
    Texture *texture;
    int x;
    int y;

    AABB aabb;
    EntityType type;
};
