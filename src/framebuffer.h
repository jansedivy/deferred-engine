#pragma once

#include <GL/glew.h>

class FrameBuffer {
  public:
    GLuint id;
    GLuint texture;

    GLuint normalTexture;
    GLuint positionTexture;
    GLuint depthTexture;

    void init(int width, int height);
    void bind();
    void bindForReading();
    void disable();

    int width;
    int height;
};
