#pragma once

#include "renderer.h"
#include "mesh.h"

class Primitives {
  public:
    Primitives();

    void set_renderer(Renderer *gl);

    Mesh* getCube();
    Mesh *getQuad();
    Mesh *get_sphere();
  private:
    Mesh *cube;
    Mesh *quad;
    Mesh *sphere;

    Renderer *gl;
};
