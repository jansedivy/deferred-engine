#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <vector>

struct DebugCommon {
  int lifeTime;
};

struct DebugLine {
  glm::vec3 start;
  glm::vec3 end;
};

class DebugDraw {
  public:
    void init();
    GLuint vbo;
    void addLine(glm::vec3 start, glm::vec3 end, int lifetime);
    void draw();

    std::vector<DebugCommon> commonData;
    std::vector<DebugLine> lines;
};
