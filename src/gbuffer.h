#pragma once

#include <GL/glew.h>

#include "common.h"

enum kGBufferTexture {
  kDiffuseTexturePosition,
  kNormalTexturePosition,
  kSpecularTexturePosition,
  kFinalTexturePosition
};

class GBuffer {
  public:
    GLuint id;
    GLuint texture;

    GLuint normalTexture;
    GLuint depthTexture;
    GLuint specularTexture;
    GLuint final_kexture;

    void init(int width, int height);
    void bind_for_writing();
    void bindForReading();
    void disable();

    void bindForLight();

    int width;
    int height;

    GLenum buffers[4];
};
