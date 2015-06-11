#include "frustum.h"
#include <stdio.h>

void Frustum::setMatrix(glm::mat4 matrix) {
  planes[kLeftPlane].normal.x = matrix[0][3] + matrix[0][0];
  planes[kLeftPlane].normal.y = matrix[1][3] + matrix[1][0];
  planes[kLeftPlane].normal.z = matrix[2][3] + matrix[2][0];
  planes[kLeftPlane].distance = matrix[3][3] + matrix[3][0];

  planes[kRightPlane].normal.x = matrix[0][3] - matrix[0][0];
  planes[kRightPlane].normal.y = matrix[1][3] - matrix[1][0];
  planes[kRightPlane].normal.z = matrix[2][3] - matrix[2][0];
  planes[kRightPlane].distance = matrix[3][3] - matrix[3][0];

  planes[kTopPlane].normal.x = matrix[0][3] - matrix[0][1];
  planes[kTopPlane].normal.y = matrix[1][3] - matrix[1][1];
  planes[kTopPlane].normal.z = matrix[2][3] - matrix[2][1];
  planes[kTopPlane].distance = matrix[3][3] - matrix[3][1];

  planes[kBottomPlane].normal.x = matrix[0][3] + matrix[0][1];
  planes[kBottomPlane].normal.y = matrix[1][3] + matrix[1][1];
  planes[kBottomPlane].normal.z = matrix[2][3] + matrix[2][1];
  planes[kBottomPlane].distance = matrix[3][3] + matrix[3][1];

  planes[kNearPlane].normal.x = matrix[0][3] + matrix[0][2];
  planes[kNearPlane].normal.y = matrix[1][3] + matrix[1][2];
  planes[kNearPlane].normal.z = matrix[2][3] + matrix[2][2];
  planes[kNearPlane].distance = matrix[3][3] + matrix[3][2];

  planes[kFarPlane].normal.x = matrix[0][3] - matrix[0][2];
  planes[kFarPlane].normal.y = matrix[1][3] - matrix[1][2];
  planes[kFarPlane].normal.z = matrix[2][3] - matrix[2][2];
  planes[kFarPlane].distance = matrix[3][3] - matrix[3][2];

  for(int i=0; i<6; i++) {
    planes[i].normalize();
  }
}

bool Frustum::sphere_in_frustum(glm::vec3 position, float radius) {
  for (int i=0; i<6; i++) {
    float distance = planes[i].distanceTo(position);
    if (distance + radius < 0) {
      return false;
    }
  }

  return true;
}
