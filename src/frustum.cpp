#include "frustum.h"

void Frustum::setMatrix(glm::mat4 matrix) {
  planes[0].normal.x = matrix[0][3] + matrix[0][0];
  planes[0].normal.y = matrix[1][3] + matrix[1][0];
  planes[0].normal.z = matrix[2][3] + matrix[2][0];
  planes[0].distance = matrix[3][3] + matrix[3][0];

  planes[1].normal.x = matrix[0][3] - matrix[0][0];
  planes[1].normal.y = matrix[1][3] - matrix[1][0];
  planes[1].normal.z = matrix[2][3] - matrix[2][0];
  planes[1].distance = matrix[3][3] - matrix[3][0];

  planes[2].normal.x = matrix[0][3] - matrix[0][1];
  planes[2].normal.y = matrix[1][3] - matrix[1][1];
  planes[2].normal.z = matrix[2][3] - matrix[2][1];
  planes[2].distance = matrix[3][3] - matrix[3][1];

  planes[3].normal.x = matrix[0][3] + matrix[0][1];
  planes[3].normal.y = matrix[1][3] + matrix[1][1];
  planes[3].normal.z = matrix[2][3] + matrix[2][1];
  planes[3].distance = matrix[3][3] + matrix[3][1];

  planes[4].normal.x = matrix[0][3] + matrix[0][2];
  planes[4].normal.y = matrix[1][3] + matrix[1][2];
  planes[4].normal.z = matrix[2][3] + matrix[2][2];
  planes[4].distance = matrix[3][3] + matrix[3][2];

  planes[5].normal.x = matrix[0][3] - matrix[0][2];
  planes[5].normal.y = matrix[1][3] - matrix[1][2];
  planes[5].normal.z = matrix[2][3] - matrix[2][2];
  planes[5].distance = matrix[3][3] - matrix[3][2];

  for(int i=0; i<6; i++) {
    planes[i].normalize();
  }
}

bool Frustum::sphereInFrustum(glm::vec3 position, float radius) {
  for (int i=0; i<6; i++) {
    float distance = planes[i].distanceTo(position);
    if (distance < -radius) {
      return false;
    }
  }

  return true;
}
