#include "entity.h"

float Entity::getBoundingRadius() {
  float x = mesh->boundingRadius * scale.x;
  float y = mesh->boundingRadius * scale.y;
  float z = mesh->boundingRadius * scale.z;

  float result = fmax(fmax(x, y), z);

  return result;
}
