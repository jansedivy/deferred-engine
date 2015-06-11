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
    LightType type;
    glm::vec3 color;
    bool isCastingShadow = false;
    FrameBuffer frame_buffer;
    Camera camera;

    glm::vec3 direction;

    glm::vec3 position;
    float radius = 500;
    float attenuation = 2;

    bool dynamic = false;
    bool already_rendered = false;
};
