#include "renderer.h"

void Renderer::init(int w, int h) {
  width = w;
  height = h;

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  /* glClearColor(31.0f/255.0f, 0.0f, 54.0f/255.0f, 1.0f); */
  glClearColor(0.0, 0.0, 0.0, 1.0);
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
  if (currentMesh && currentMesh == mesh) {
    return;
  }

  currentMesh = mesh;

  if (shader->attributes.count("normals")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.normals);
    glVertexAttribPointer(shader->attributes["normals"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shader->attributes.count("uv")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.uvs);
    glVertexAttribPointer(shader->attributes["uv"], 2, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shader->attributes.count("position")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.vertices);
    glVertexAttribPointer(shader->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);
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
  shader = &skyboxShader;
  shader->use();

  glm::mat4 projection = glm::perspective(glm::radians(camera->fov), camera->aspectRatio, camera->near, camera->far);
  glm::mat4 view;

  view = glm::rotate(view, camera->rotation.x, glm::vec3(1.0, 0.0, 0.0));
  view = glm::rotate(view, camera->rotation.y, glm::vec3(0.0, 1.0, 0.0));
  view = glm::rotate(view, camera->rotation.z, glm::vec3(0.0, 0.0, 1.0));

  shader->mat4("view", view);
  shader->mat4("projection", projection);
  shader->cubemap("uSampler", skybox->texture->id, 0);

  glBindBuffer(GL_ARRAY_BUFFER, skybox->mesh->buffers.vertices);
  glVertexAttribPointer(shader->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->mesh->buffers.indices);
  glDrawElements(GL_TRIANGLES, skybox->mesh->indices.size(), GL_UNSIGNED_INT, 0);

  shader->disable();

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::debugRenderFrameBuffer(FrameBuffer *framebuffer, Mesh *fullscreenMesh) {
  shader = &fullscreen;
  shader->use();

    useMesh(fullscreenMesh);

    framebuffer->bindForReading();
    glViewport(0, 0, width/2, height/2);
    shader->texture("texture", framebuffer->texture, 0);
    draw();

    glViewport(width/2, 0, width/2, height/2);
    shader->texture("texture", framebuffer->normalTexture, 0);
    draw();

    glViewport(0, height/2, width/2, height/2);
    shader->texture("texture", framebuffer->positionTexture, 0);
    draw();

    glViewport(width/2, height/2, width/2, height/2);
    shader->texture("texture", framebuffer->depthTexture, 0);
    draw();

  shader->disable();
}
