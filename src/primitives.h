#pragma once

#include "renderer.h"
#include "mesh.h"

class Primitives {
  public:
    Primitives();

    void setRenderer(Renderer *gl);

    Mesh* getCube();
    Mesh *getQuad();
    Mesh *getSphere();
  private:
    Mesh *cube;
    Mesh *quad;
    Mesh *sphere;

    Renderer *gl;
};
