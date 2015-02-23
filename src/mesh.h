#pragma once

#include <GL/glew.h>

#include <vector>
#include "aabb.h"

struct MeshBuffers {
  GLuint indices;
  GLuint vertices;
  GLuint normals;
  GLuint uvs;
};

struct Mesh {
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> uv;
  std::vector<GLuint> indices;
  float boundingRadius;

  MeshBuffers buffers;
};

