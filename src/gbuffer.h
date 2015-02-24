#pragma once

#include <GL/glew.h>

#include "common.h"

enum kGBufferTexture {
  kDiffuseTexturePosition = 0,
  kNormalTexturePosition = 1,
  kPositionTexturePosition = 2,
  kFinalTexturePosition = 4
};

class GBuffer {
  public:
    GLuint id;
    GLuint texture;

    GLuint normalTexture;
    GLuint positionTexture;
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
