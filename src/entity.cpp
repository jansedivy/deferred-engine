#include "entity.h"

Sphere Entity::get_bounding_sphere() {
  Sphere result;

  float x = mesh->bounding_radius * scale.x;
  float y = mesh->bounding_radius * scale.y;
  float z = mesh->bounding_radius * scale.z;

  result.radius = std::max(std::max(x, y), z);
  result.position = position + mesh->relative_center;

  return result;
}
