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
    Light() {
      isCastingShadow = false;
      radius = 500.0f;
    };

    glm::vec3 color;
    glm::vec3 direction;
    glm::vec3 position;
    float radius;
    LightType type;
    bool isCastingShadow;

    FrameBuffer frameBuffer;
    Camera camera;
};
