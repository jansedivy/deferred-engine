#include "entity.h"

float Entity::getBoundingRadius() {
  float x = mesh->boundingRadius * scale.x;
  float y = mesh->boundingRadius * scale.y;
  float z = mesh->boundingRadius * scale.z;

  float result = std::max(std::max(x, y), z);

  return result;
}
