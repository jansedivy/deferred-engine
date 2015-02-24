#include "gbuffer.h"

void GBuffer::init(int w, int h) {
  width = w;
  height = h;

  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

  // diffuse
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + kDiffuseTexturePosition, GL_TEXTURE_2D, texture, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // normal
  glGenTextures(1, &normalTexture);
  glBindTexture(GL_TEXTURE_2D, normalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + kNormalTexturePosition, GL_TEXTURE_2D, normalTexture, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // position
  glGenTextures(1, &positionTexture);
  glBindTexture(GL_TEXTURE_2D, positionTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + kPositionTexturePosition, GL_TEXTURE_2D, positionTexture, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // final
  glGenTextures(1, &finalTexture);
  glBindTexture(GL_TEXTURE_2D, finalTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + kFinalTexturePosition, GL_TEXTURE_2D, finalTexture, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenTextures(1, &depthTexture);
  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  buffers[0] = GL_COLOR_ATTACHMENT0 + kDiffuseTexturePosition;
  buffers[1] = GL_COLOR_ATTACHMENT0 + kNormalTexturePosition;
  buffers[2] = GL_COLOR_ATTACHMENT0 + kPositionTexturePosition;
  buffers[3] = GL_COLOR_ATTACHMENT0 + kFinalTexturePosition;

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GBuffer::bindForWriting() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
  glDrawBuffers(arrayCount(buffers), buffers);
}

void GBuffer::bindForReading() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
  glReadBuffer(GL_COLOR_ATTACHMENT0 + kFinalTexturePosition);
}

void GBuffer::bindForLight() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
  glDrawBuffer(GL_COLOR_ATTACHMENT0 + kFinalTexturePosition);
}

void GBuffer::disable() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
