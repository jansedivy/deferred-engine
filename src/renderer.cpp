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

  anti_alias = true;
  ssao = true;
  ssao_radius = 5.0f;
  fog = true;
  bloom = true;

  glewExperimental = GL_TRUE;
  glewInit();

  enable_depth_read();
  glEnable(GL_DEPTH_CLAMP);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glClearColor(0.0, 0.0, 0.0, 1.0);

  kernel_size = 32;

  kernel = new glm::vec3[kernel_size];
  for (int i=0; i<kernel_size; i++) {
    glm::vec3 value = glm::vec3(
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(0.0f, 1.0f));

    value = glm::normalize(value);

    float scale = float(i) / float(kernel_size);
    value = value * lerp(0.1f, 1.0f, scale * scale);

    kernel[i] = value;
  }

  noise_size = 4;
  for (int i=0; i<(noise_size*noise_size); i++) {
    noise[i] = glm::vec3(
        getRandomFloat(-1.0f, 1.0f),
        getRandomFloat(-1.0f, 1.0f),
        0.0f);
    noise[i] = glm::normalize(noise[i]);
  }

  glGenTextures(1, &noise_id);
  glBindTexture(GL_TEXTURE_2D, noise_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, noise_size, noise_size, 0, GL_RGB, GL_FLOAT, noise);
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

  glGenBuffers(1, &screen_aligned_quad);
  glBindBuffer(GL_ARRAY_BUFFER, screen_aligned_quad);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  gbuffer.init(width, height);
}

void Renderer::populate_buffers(Mesh *mesh) {
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

void Renderer::bind_mesh(Mesh *mesh) {
  current_mesh = mesh;

  if (shader_manager.current->attributes.count("normals")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.normals);
    glVertexAttribPointer(shader_manager.current->attributes["normals"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shader_manager.current->attributes.count("tangents")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.tangents);
    glVertexAttribPointer(shader_manager.current->attributes["tangents"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shader_manager.current->attributes.count("uv")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.uvs);
    glVertexAttribPointer(shader_manager.current->attributes["uv"], 2, GL_FLOAT, GL_FALSE, 0, 0);
  }

  if (shader_manager.current->attributes.count("position")) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.vertices);
    glVertexAttribPointer(shader_manager.current->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers.indices);
}

void Renderer::draw(bool wireframe) {
  int size = current_mesh->indices.size();
  if (wireframe) {
    glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
  } else {
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
  }
}

void Renderer::draw_skybox(Skybox *skybox, Camera *camera) {
  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_NOTEQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  disable_depth_read();
  disable_depth_write();

  shader_manager.use("skybox");

  glm::mat4 projection = glm::perspective(glm::radians(camera->fov), camera->aspect_ratio, camera->near, camera->far);
  glm::mat4 view;

  view = glm::rotate(view, camera->rotation.x, glm::vec3(1.0, 0.0, 0.0));
  view = glm::rotate(view, camera->rotation.y, glm::vec3(0.0, 1.0, 0.0));
  view = glm::rotate(view, camera->rotation.z, glm::vec3(0.0, 0.0, 1.0));

  shader_manager.current->set_uniform("view", view);
  shader_manager.current->set_uniform("projection", projection);
  shader_manager.current->cubemap("uSampler", skybox->texture->id, 0);

  glBindBuffer(GL_ARRAY_BUFFER, skybox->mesh->buffers.vertices);
  glVertexAttribPointer(shader_manager.current->attributes["position"], 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->mesh->buffers.indices);
  glDrawElements(GL_TRIANGLES, skybox->mesh->indices.size(), GL_UNSIGNED_INT, 0);

  shader_manager.current->disable();

  enable_depth_write();
  enable_depth_read();
  glDisable(GL_STENCIL_TEST);
}

void Renderer::render_fullscreen_texture(GLuint texture) {
  glDisable(GL_CULL_FACE);
  shader_manager.use("fullscreen");
    glViewport(0, 0, width, height);
    shader_manager.current->texture("uSampler", texture, 0);

    bind_screen_aligned_quad();
    draw_screen_aligned_quad();
  shader_manager.current->disable();
}

void Renderer::debug_renderer_gbuffer(GBuffer *framebuffer) {
  shader_manager.use("fullscreen");
    bind_screen_aligned_quad();

    framebuffer->bindForReading();

    glViewport(0, 0, width/2, height/2);
    shader_manager.current->texture("uSampler", framebuffer->texture, 0);
    draw_screen_aligned_quad();

    glViewport(width/2, 0, width/2, height/2);
    shader_manager.current->texture("uSampler", framebuffer->normalTexture, 0);
    draw_screen_aligned_quad();

  shader_manager.current->disable();
}

void Renderer::draw_lights(std::vector<Light*> *lights, Mesh *sphere, Camera *camera) {
  PROFILE("Lights");

  gbuffer.bindForLight();

  glEnable(GL_STENCIL_TEST);

  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  clear();

  gbuffer.bindForLight();
  render_point_lights(lights, sphere, camera);
  render_directional_lights(lights, camera);
  render_ambient_light(lights, camera);

  glDisable(GL_BLEND);

  PROFILE_END();
}

void Renderer::render_point_lights(std::vector<Light*> *lights, Mesh *sphere, Camera *camera) {
  PROFILE("Point Lights");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  disable_depth_read();

  shader_manager.use("pointshader");

  bind_mesh(sphere);

  glm::mat4 invProjection = glm::inverse(camera->view_matrix);

  shader_manager.current->set_uniform("invProjection", invProjection);
  shader_manager.current->set_uniform("uPMatrix", camera->view_matrix);

  shader_manager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shader_manager.current->texture("normalTexture", gbuffer.normalTexture, 1);
  shader_manager.current->texture("specularTexture", gbuffer.specularTexture, 2);
  shader_manager.current->texture("depthTexture", gbuffer.depthTexture, 3);

  shader_manager.current->set_uniform("camera", camera->position);

  for (auto it = lights->begin(); it != lights->end(); it++) {
    Light *light = *it;

    if (light->type == kPoint) {
      if (!camera->frustum.sphere_in_frustum(light->position, light->radius)) {
        continue;
      }

      glm::mat4 model_view;
      model_view = glm::translate(model_view, light->position);
      model_view = glm::scale(model_view, glm::vec3(light->radius));

      shader_manager.current->set_uniform("uMVMatrix", model_view);
      shader_manager.current->set_uniform("lightPosition", light->position);
      shader_manager.current->set_uniform("lightRadius", light->radius);
      shader_manager.current->set_uniform("lightColor", light->color);
      shader_manager.current->set_uniform("uAttenuation", light->attenuation);

      if (light->isCastingShadow) {
        shader_manager.current->set_uniform("isCastingShadow", 1);
        shader_manager.current->texture("shadowMap", light->frame_buffer.depthTexture, 4);
        shader_manager.current->set_uniform("lightMatrix", light->camera.view_matrix);
      } else {
        shader_manager.current->set_uniform("isCastingShadow", 0);
      }

      draw();
    }
  }

  shader_manager.current->disable();

  PROFILE_END();
}

void Renderer::render_ambient_light(std::vector<Light*> *lights, Camera *camera) {
  PROFILE("Ambient Light");

  glDisable(GL_CULL_FACE);

  shader_manager.use("ambientlight");

  shader_manager.current->texture("diffuseTexture", gbuffer.texture, 0);

  bind_screen_aligned_quad();

  for (auto it = lights->begin(); it != lights->end(); it++) {
    Light *light = *it;

    if (light->type == kAmbient) {
      shader_manager.current->set_uniform("lightColor", light->color);
      draw_screen_aligned_quad();
    }
  }

  shader_manager.current->disable();

  PROFILE_END();
}

void Renderer::render_directional_lights(std::vector<Light*> *lights, Camera *camera) {
  PROFILE("Directional Lights");

  glDisable(GL_CULL_FACE);

  shader_manager.use("directionlight");

  shader_manager.current->texture("diffuseTexture", gbuffer.texture, 0);
  shader_manager.current->texture("normalTexture", gbuffer.normalTexture, 1);

  bind_screen_aligned_quad();

  for (auto it = lights->begin(); it != lights->end(); it++) {
    Light *light = *it;

    if (light->type == kDirectional) {
      shader_manager.current->set_uniform("lightColor", light->color);
      shader_manager.current->set_uniform("lightDirection", light->direction);
      draw_screen_aligned_quad();
    }
  }

  shader_manager.current->disable();

  PROFILE_END();
}

void Renderer::clear(bool clear_depth) {
  if (clear_depth) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

void Renderer::enable_depth_read() {
  glEnable(GL_DEPTH_TEST);
}

void Renderer::disable_depth_read() {
  glDisable(GL_DEPTH_TEST);
}

void Renderer::enable_depth_write() {
  glDepthMask(GL_TRUE);
}

void Renderer::disable_depth_write() {
  glDepthMask(GL_FALSE);
}

void Renderer::final_render(Camera *camera) {
  PROFILE("Post processing");
  if (anti_alias) {
    PROFILE("Anti-aliasing");
    shader_manager.use("fxaa");
      shader_manager.current->texture("uSampler", gbuffer.final_kexture, 0);
      bind_screen_aligned_quad();
      draw_screen_aligned_quad();
    shader_manager.current->disable();
    PROFILE_END();
  }

  if (ssao) {
    PROFILE("SSAO");
    shader_manager.use("ssao");

    bind_screen_aligned_quad();

    glm::mat4 invProjection = glm::inverse(camera->view_matrix);
    shader_manager.current->set_uniform("invProjection", invProjection);
    shader_manager.current->set_uniform("uPMatrix", camera->view_matrix);

    glUniform3fv(shader_manager.current->uniforms["kernel[0]"], kernel_size, (const GLfloat *)kernel);

    shader_manager.current->texture("uDepth", gbuffer.depthTexture, 0);
    shader_manager.current->texture("uNormal", gbuffer.normalTexture, 1);
    shader_manager.current->texture("noiseTexture", noise_id, 2);
    shader_manager.current->texture("diffuseTexture", gbuffer.final_kexture, 4);

    shader_manager.current->set_uniform("noiseScale", noise_size);
    shader_manager.current->set_uniform("uKernelSize", kernel_size);
    shader_manager.current->set_uniform("uRadius", ssao_radius);

    shader_manager.current->set_uniform("zNear", camera->near);
    shader_manager.current->set_uniform("zFar", camera->far);

    draw_screen_aligned_quad();

    shader_manager.current->disable();
    PROFILE_END();
  }

  if (fog) {
    PROFILE("Fog");

    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    shader_manager.use("fog");
      bind_screen_aligned_quad();

      shader_manager.current->set_uniform("zNear", camera->near);
      shader_manager.current->set_uniform("zFar", camera->far);

      shader_manager.current->set_uniform("fogColor", camera->fog_color);
      shader_manager.current->set_uniform("density", camera->fog_density);

      shader_manager.current->texture("uDepth", gbuffer.depthTexture, 0);
      shader_manager.current->texture("diffuseTexture", gbuffer.final_kexture, 4);

      draw_screen_aligned_quad();
    shader_manager.current->disable();

    glDisable(GL_STENCIL_TEST);

    PROFILE_END();
  }

  if (bloom) {
    PROFILE("Bloom");

    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    shader_manager.use("bloom");
      bind_screen_aligned_quad();
      shader_manager.current->texture("diffuseTexture", gbuffer.final_kexture, 0);
      draw_screen_aligned_quad();
    shader_manager.current->disable();

    glDisable(GL_STENCIL_TEST);

    PROFILE_END();
  }

  PROFILE("Final");
  gbuffer.bindForReading();
  render_fullscreen_texture(gbuffer.final_kexture);
  PROFILE_END();

  PROFILE_END();
}

void Renderer::bind_screen_aligned_quad() {
  glBindBuffer(GL_ARRAY_BUFFER, screen_aligned_quad);
  glVertexAttribPointer(shader_manager.current->attributes["position"], 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Renderer::draw_screen_aligned_quad() {
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
