#pragma once

#include <GL/glew.h>

#include "common.h"

enum kGBufferTexture {
  kDiffuseTexturePosition,
  kNormalTexturePosition,
  kFinalTexturePosition
};

class GBuffer {
  public:
    GLuint id;
    GLuint texture;

    GLuint normalTexture;
    GLuint depthTexture;
    GLuint finalTexture;

    void init(int width, int height);
    void bindForWriting();
    void bindForReading();
    void disable();

    void bindForLight();

    int width;
    int height;

    GLenum buffers[4];
};
