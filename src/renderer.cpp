#include "renderer.h"

float getRandomFloat(float min, float max) {
  return ((float)rand())/((float)RAND_MAX) * (max - min) + min;
}

float lerp(float start, float end, float time) {
  return start + (end - start) * time;
}

void Renderer::init(int w, int h) {
  width = w;
  height = h;

  antiAlias = true;
  ssao = true;
  ssaoRadius = 5.0f;

  glewExperimental = GL_TRUE;
  glewInit();

  enableDepthRead();
  glEnable(GL_DEPTH_CLAMP);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glClearColor(0.0, 0.0, 0.0, 1.0);

  kernelSize = 32;

  kernel = new glm::vec3[kernelSize];
  for (int i=0; i<kernelSize; i++) {
    glm::vec3 value = glm::vec3(
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(0.0f, 1.0f));

    value = glm::normalize(value);

    float scale = float(i) / float(kernelSize);
    value = value * lerp(0.1f, 1.0f, scale * scale);

    kernel[i] = value;
  }

  noiseSize = 4;
  for (int i=0; i<(noiseSize*noiseSize); i++) {
    noise[i] = glm::vec3(
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(-1.0f, 1.0f),
        0.0f);
    noise[i] = glm::normalize(noise[i]);
  }

  glGenTextures(1, &noiseId);
  glBindTexture(GL_TEXTURE_2D, noiseId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, noise);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  float vertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,

    1.0f, -1.0f,
    1.1f, 1.0f,
    -1.0f, 1.0f
  };

  glGenBuffers(1, &screenAlignedQuad);
  glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  gbuffer.init(width, height);
}

void Renderer::populateBuffers(Mesh *mesh) {
  glGenBuffers(1, &mesh->buffers.indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers.indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLuint), &mesh->indices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &mesh->buffers.vertices);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.vertices);
  glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float), &mesh->vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &mesh->buffers.uvs);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.uvs);
  glBufferData(GL_ARRAY_BUFFER, mesh->uv.size() * sizeof(float), &mesh->uv[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &mesh->buffers.normals);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.normals);
  glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(float), &mesh->normals[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &mesh->buffers.tangents);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.tangents);
  glBufferData(GL_ARRAY_BUFFER, mesh->tangents.size() * sizeof(float), &mesh->tangents[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::bindMesh(Mesh *mesh) {
  currentMesh = mesh;

  if (shaderManager.current->attributes.count("normals")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.normals);
    glVertexAttribPointer(shaderManager.current->attributes["normals"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shaderManager.current->attributes.count("tangents")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.tangents);
    glVertexAttribPointer(shaderManager.current->attributes["tangents"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shaderManager.current->attributes.count("uv")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.uvs);
    glVertexAttribPointer(shaderManager.current->attributes["uv"], 2, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shaderManager.current->attributes.count("position")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.vertices);
    glVertexAttribPointer(shaderManager.current->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers.indices);
}

void Renderer::draw(bool wireframe) {
  int size = currentMesh->indices.size();
  if (wireframe) {
    glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
  } else {
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
  }
}

void Renderer::drawSkybox(Skybox *skybox, Camera *camera) {
  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_NOTEQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  disableDepthRead();
  disableDepthWrite();

  shaderManager.use("skybox");

  glm::mat4 projection = glm::perspective(glm::radians(camera->fov), camera->aspectRatio, camera->near, camera->far);
  glm::mat4 view;

  view = glm::rotate(view, camera->rotation.x, glm::vec3(1.0, 0.0, 0.0));
  view = glm::rotate(view, camera->rotation.y, glm::vec3(0.0, 1.0, 0.0));
  view = glm::rotate(view, camera->rotation.z, glm::vec3(0.0, 0.0, 1.0));

  shaderManager.current->setUniform("view", view);
  shaderManager.current->setUniform("projection", projection);
  shaderManager.current->cubemap("uSampler", skybox->texture->id, 0);

  glBindBuffer(GL_ARRAY_BUFFER, skybox->mesh->buffers.vertices);
  glVertexAttribPointer(shaderManager.current->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->mesh->buffers.indices);
  glDrawElements(GL_TRIANGLES, skybox->mesh->indices.size(), GL_UNSIGNED_INT, 0);

  shaderManager.current->disable();

  enableDepthWrite();
  enableDepthRead();
  glDisable(GL_STENCIL_TEST);
}

void Renderer::renderFullscreenTexture(GLuint texture) {
  glDisable(GL_CULL_FACE);
  shaderManager.use("fullscreen");
    glViewport(0, 0, width, height);
    shaderManager.current->texture("uSampler", texture, 0);

    glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
    glVertexAttribPointer(shaderManager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  shaderManager.current->disable();
}

void Renderer::debugRendererGBuffer(GBuffer *framebuffer) {
  shaderManager.use("fullscreen");
    glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
    glVertexAttribPointer(shaderManager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);

    framebuffer->bindForReading();

    glViewport(0, 0, width/2, height/2);
    shaderManager.current->texture("uSampler", framebuffer->texture, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glViewport(width/2, 0, width/2, height/2);
    shaderManager.current->texture("uSampler", framebuffer->normalTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glViewport(0, height/2, width/2, height/2);
    shaderManager.current->texture("uSampler", framebuffer->positionTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glViewport(width/2, height/2, width/2, height/2);
    shaderManager.current->texture("uSampler", framebuffer->depthTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

  shaderManager.current->disable();
}

void Renderer::drawLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera) {
  profiler->start("Lights");

  gbuffer.bindForLight();

  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  clear();

  gbuffer.bindForLight();
  renderPointLights(lights, profiler, sphere, camera);
  renderDirectionalLights(lights, profiler, camera);

  glDisable(GL_BLEND);

  profiler->end();
}

void Renderer::renderPointLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera) {
  profiler->start("Point Lights");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  disableDepthRead();

  shaderManager.use("pointshader");

  bindMesh(sphere);

  glm::mat4 invProjection = glm::inverse(camera->viewMatrix);

  shaderManager.current->setUniform("invProjection", invProjection);
  shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

  shaderManager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shaderManager.current->texture("normalTexture", gbuffer.normalTexture, 1);
  shaderManager.current->texture("positionTexture", gbuffer.positionTexture, 2);
  shaderManager.current->texture("depthTexture", gbuffer.depthTexture, 3);

  shaderManager.current->setUniform("camera", camera->position);

  for (auto it = lights->begin(); it != lights->end(); it++) {
    if (it->type == kPoint) {
      if (!camera->frustum.sphereInFrustum(it->position, it->radius)) {
        continue;
      }

      glm::mat4 modelView;
      modelView = glm::translate(modelView, it->position);
      modelView = glm::scale(modelView, glm::vec3(it->radius));

      shaderManager.current->setUniform("uMVMatrix", modelView);
      shaderManager.current->setUniform("lightPosition", it->position);
      shaderManager.current->setUniform("lightRadius", it->radius);
      shaderManager.current->setUniform("lightColor", it->color);

      draw();
    }
  }

  shaderManager.current->disable();

  profiler->end();
}

void Renderer::renderDirectionalLights(std::vector<Light> *lights, Profiler *profiler, Camera *camera) {
  profiler->start("Directional Lights");

  glDisable(GL_CULL_FACE);

  shaderManager.use("directionlight");

  shaderManager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shaderManager.current->texture("normalTexture", gbuffer.normalTexture, 1);
  shaderManager.current->texture("positionTexture", gbuffer.positionTexture, 2);

  glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
  glVertexAttribPointer(shaderManager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);

  for (auto it = lights->begin(); it != lights->end(); it++) {
    if (it->type == kDirectional) {
      shaderManager.current->setUniform("lightColor", it->color);
      shaderManager.current->setUniform("lightDirection", it->direction);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }
  }

  shaderManager.current->disable();

  profiler->end();
}

void Renderer::clear(bool clearDepth) {
  if (clearDepth) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

void Renderer::enableDepthRead() {
  glEnable(GL_DEPTH_TEST);
}

void Renderer::disableDepthRead() {
  glDisable(GL_DEPTH_TEST);
}

void Renderer::enableDepthWrite() {
  glDepthMask(GL_TRUE);
}

void Renderer::disableDepthWrite() {
  glDepthMask(GL_FALSE);
}

void Renderer::finalRender(Profiler *profiler, Camera *camera) {
  if (antiAlias) {
    profiler->start("Anti-aliasing");
    shaderManager.use("fxaa");
      shaderManager.current->texture("uSampler", gbuffer.finalTexture, 0);
      glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
      glVertexAttribPointer(shaderManager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderManager.current->disable();
    profiler->end();
  }

  if (ssao) {
    profiler->start("SSAO");
    shaderManager.use("ssao");

    glBindBuffer(GL_ARRAY_BUFFER, screenAlignedQuad);
    glVertexAttribPointer(shaderManager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);

    glm::mat4 invProjection = glm::inverse(camera->viewMatrix);
    shaderManager.current->setUniform("invProjection", invProjection);
    shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

    glUniform3fv(shaderManager.current->uniforms["kernel[0]"], kernelSize, (const GLfloat *)kernel);

    shaderManager.current->texture("uDepth", gbuffer.depthTexture, 0);
    shaderManager.current->texture("uNormal", gbuffer.normalTexture, 1);
    shaderManager.current->texture("noiseTexture", noiseId, 2);
    shaderManager.current->texture("uPosition", gbuffer.positionTexture, 3);
    shaderManager.current->texture("diffuseTexture", gbuffer.finalTexture, 4);

    shaderManager.current->setUniform("noiseScale", noiseSize);
    shaderManager.current->setUniform("uKernelSize", kernelSize);
    shaderManager.current->setUniform("uRadius", ssaoRadius);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderManager.current->disable();
    profiler->end();
  }

  gbuffer.bindForReading();
  renderFullscreenTexture(gbuffer.finalTexture);
}
