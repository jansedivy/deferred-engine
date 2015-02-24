#include "renderer.h"

void Renderer::init(int w, int h) {
  width = w;
  height = h;

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glClearColor(0.0, 0.0, 0.0, 1.0);

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
}

void Renderer::useMesh(Mesh *mesh) {
  currentMesh = mesh;

  if (shaderManager.current->attributes.count("normals")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.normals);
    glVertexAttribPointer(shaderManager.current->attributes["normals"], 3, GL_FLOAT, GL_FALSE, 0, 0);
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
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
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

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::renderFullscreenTexture(GLuint texture, Mesh *fullscreenMesh) {
  glDisable(GL_CULL_FACE);
  shaderManager.use("fullscreen");
    useMesh(fullscreenMesh);
    glViewport(0, 0, width, height);
    shaderManager.current->texture("texture", texture, 0);
    draw();
  shaderManager.current->disable();
}

void Renderer::debugRendererGBuffer(GBuffer *framebuffer, Mesh *fullscreenMesh) {
  shaderManager.use("fullscreen");

    useMesh(fullscreenMesh);
    framebuffer->bindForReading();

    glViewport(0, 0, width/2, height/2);
    shaderManager.current->texture("texture", framebuffer->texture, 0);
    draw();

    glViewport(width/2, 0, width/2, height/2);
    shaderManager.current->texture("texture", framebuffer->normalTexture, 0);
    draw();

    glViewport(0, height/2, width/2, height/2);
    shaderManager.current->texture("texture", framebuffer->positionTexture, 0);
    draw();

    glViewport(width/2, height/2, width/2, height/2);
    shaderManager.current->texture("texture", framebuffer->depthTexture, 0);
    draw();

  shaderManager.current->disable();
}

void Renderer::drawLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Mesh *fullscreenMesh, Camera *camera) {
  profiler->start("Lights");

  gbuffer.bindForLight();

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  glClear(GL_COLOR_BUFFER_BIT);

  gbuffer.bindForLight();
  renderPointLights(lights, profiler, sphere, camera);
  renderDirectionalLights(lights, profiler, fullscreenMesh, camera);

  glDisable(GL_BLEND);

  profiler->end();
}

void Renderer::renderPointLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera) {
  profiler->start("Point Lights");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glDisable(GL_DEPTH_TEST);

  shaderManager.use("pointshader");

  useMesh(sphere);

  glm::mat4 invProjection = glm::inverse(camera->viewMatrix);

  shaderManager.current->setUniform("invProjection", invProjection);
  shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

  shaderManager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shaderManager.current->texture("normalTexture", gbuffer.normalTexture, 1);
  shaderManager.current->texture("positionTexture", gbuffer.positionTexture, 2);
  shaderManager.current->texture("depthTexture", gbuffer.depthTexture, 3);

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

void Renderer::renderDirectionalLights(std::vector<Light> *lights, Profiler *profiler, Mesh *fullscreenMesh, Camera *camera) {
  profiler->start("Directional Lights");

  glDisable(GL_CULL_FACE);

  shaderManager.use("directionlight");

  shaderManager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shaderManager.current->texture("normalTexture", gbuffer.normalTexture, 1);
  shaderManager.current->texture("positionTexture", gbuffer.positionTexture, 2);

  useMesh(fullscreenMesh);

  for (auto it = lights->begin(); it != lights->end(); it++) {
    if (it->type == kDirectional) {
      shaderManager.current->setUniform("lightColor", it->color);
      shaderManager.current->setUniform("lightDirection", it->direction);
      draw();
    }
  }

  shaderManager.current->disable();

  profiler->end();
}
