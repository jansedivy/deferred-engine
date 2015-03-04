#pragma once

#include <GL/glew.h>

class FrameBuffer {
  public:
    void init(int width, int height);

    void bind();
    void unbind();

    GLuint id;
    GLuint texture;
    GLuint depthTexture;

    int width;
    int height;
};