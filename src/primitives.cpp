#include "primitives.h"

Primitives::Primitives() {
  cube = 0;
  quad = 0;
  sphere = 0;
}

Mesh *Primitives::getSphere() {
  if (sphere) { return sphere; }

  Mesh *mesh = new Mesh();

  double latitudeBands = 30;
  double longitudeBands = 30;
  double radius = 1.0;

  for (double latNumber = 0; latNumber <= latitudeBands; latNumber++) {
    double theta = latNumber * M_PI / latitudeBands;
    double sinTheta = sin(theta);
    double cosTheta = cos(theta);

    for (double longNumber = 0; longNumber <= longitudeBands; longNumber++) {
      double phi = longNumber * 2 * M_PI / longitudeBands;
      double sinPhi = sin(phi);
      double cosPhi = cos(phi);

      double x = cosPhi * sinTheta;
      double y = cosTheta;
      double z = sinPhi * sinTheta;

      mesh->normals.push_back(x);
      mesh->normals.push_back(y);
      mesh->normals.push_back(z);

      mesh->uv.push_back(1 - (longNumber / longitudeBands));
      mesh->uv.push_back(1 - (latNumber / latitudeBands));

      mesh->vertices.push_back(radius * x);
      mesh->vertices.push_back(radius * y);
      mesh->vertices.push_back(radius * z);
    }
  }

  for (int latNumber = 0; latNumber < latitudeBands; latNumber++) {
    for (int longNumber = 0; longNumber < longitudeBands; longNumber++) {
      int first = (latNumber * (longitudeBands + 1)) + longNumber;
      int second = first + longitudeBands + 1;

      mesh->indices.push_back(first);
      mesh->indices.push_back(second);
      mesh->indices.push_back(first + 1);

      mesh->indices.push_back(second);
      mesh->indices.push_back(second + 1);
      mesh->indices.push_back(first + 1);
    }
  }

  renderer->populateBuffers(mesh);

  mesh->boundingRadius = radius;

  sphere = mesh;

  return mesh;
}

Mesh* Primitives::getCube() {
  if (cube) {
    return cube;
  }

  GLuint indices[] = {
    0, 1, 2,      0, 2, 3,
    4, 5, 6,      4, 6, 7,
    8, 9, 10,     8, 10, 11,
    12, 13, 14,   12, 14, 15,
    16, 17, 18,   16, 18, 19,
    20, 21, 22,   20, 22, 23
  };

  float vertices[] = {
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
    1.0,  1.0, -1.0,
    1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
    -1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,
    1.0,  1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0, -1.0,  1.0,
    -1.0, -1.0,  1.0,
    1.0, -1.0, -1.0,
    1.0,  1.0, -1.0,
    1.0,  1.0,  1.0,
    1.0, -1.0,  1.0,
    -1.0, -1.0, -1.0,
    -1.0, -1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0
  };

  float normals[] = {
    0.0,  0.0,  1.0,
    0.0,  0.0,  1.0,
    0.0,  0.0,  1.0,
    0.0,  0.0,  1.0,
    0.0,  0.0, -1.0,
    0.0,  0.0, -1.0,
    0.0,  0.0, -1.0,
    0.0,  0.0, -1.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0,  1.0,  0.0,
    0.0, -1.0,  0.0,
    0.0, -1.0,  0.0,
    0.0, -1.0,  0.0,
    0.0, -1.0,  0.0,
    1.0,  0.0,  0.0,
    1.0,  0.0,  0.0,
    1.0,  0.0,  0.0,
    1.0,  0.0,  0.0,
    -1.0,  0.0,  0.0,
    -1.0,  0.0,  0.0,
    -1.0,  0.0,  0.0,
    -1.0,  0.0,  0.0
  };

  float uv[] = { 0, -3, 1, -3, 0, -2, 1, -2, 0, -2, 1, -2, 0, -1, 1, -1, 0, -1, 1, -1,
     0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, -3, 2, -3, 1, -2, 2, -2, -1, -3, 0,
     -3, -1, -2, 0, -2 };

  Mesh *mesh = new Mesh();
  mesh->vertices.assign(std::begin(vertices), std::end(vertices));
  mesh->indices.assign(std::begin(indices), std::end(indices));
  mesh->normals.assign(std::begin(normals), std::end(normals));
  mesh->uv.assign(std::begin(uv), std::end(uv));

  renderer->populateBuffers(mesh);
  mesh->boundingRadius = 1.0;

  cube = mesh;

  return mesh;
}

Mesh* Primitives::getQuad() {
  if (quad) {
    return quad;
  }

  GLuint fullscreenIndices[] = { 0, 1, 2, 0, 2, 3 };
  float fullscreenVertices[] = { -1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, -1.0, 1.0, 0.0 };
  float fullscreenUV[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0 };
  Mesh *mesh = new Mesh();
  mesh->vertices = std::vector<float>(fullscreenVertices, std::end(fullscreenVertices));
  mesh->indices = std::vector<GLuint>(fullscreenIndices, std::end(fullscreenIndices));
  mesh->uv = std::vector<float>(fullscreenUV, std::end(fullscreenUV));
  renderer->populateBuffers(mesh);

  quad = mesh;

  return mesh;
}
