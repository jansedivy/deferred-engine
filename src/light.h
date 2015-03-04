#pragma once

#include <glm/vec3.hpp>

#include "frame_buffer.h"
#include "camera.h"

enum LightType {
  kPoint,
  kDirectional,
  kAmbient
};

class Light {
  public:
    Light();
    LightType type;
    glm::vec3 color;
    bool isCastingShadow;
    FrameBuffer frameBuffer;
    Camera camera;

    glm::vec3 direction;

    glm::vec3 position;
    float radius;
};
