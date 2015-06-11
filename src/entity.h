#pragma once

#include <glm/vec3.hpp>
#include <cmath>

#include "mesh.h"
#include "texture.h"
#include "sphere.h"

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
    Texture *normal_map;
    Texture *specularTexture;
    Texture *alpha_texture;

    int x;
    int y;

    AABB aabb;
    EntityType type;

    Sphere get_bounding_sphere();
};
