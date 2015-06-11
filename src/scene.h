#pragma once

#include <vector>

#include "loader.h"
#include "entity.h"
#include "light.h"
#include "renderer.h"

class Scene {
  public:
    void init(Loader *loader, Renderer *gl);

    std::vector<Light*> lights;
    std::vector<Entity*> entities;
};
