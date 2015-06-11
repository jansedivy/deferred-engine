#include "game.h"

void Game::init() {
  global_profiler = new Profiler();

  PROFILE("Game init");
  light_radius = 500.0f;
  width = 1280;
  height = (float)width * 9.0f/16.0f;

  PROFILE("SDL load");
  SDL_Init(SDL_INIT_EVERYTHING);

  char *base_path = SDL_GetBasePath();
  chdir(base_path);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  window = SDL_CreateWindow("Game",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      width, height,
      SDL_WINDOW_OPENGL);

  keyboard_state = SDL_GetKeyboardState(NULL);

  SDL_GLContext context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(-1);
  PROFILE_END();

  gl.init(width, height);

  camera.set_aspect((float)width/(float)height);

  PROFILE("loading shaders");
  gl.shader_manager.load("skybox", "shaders/skybox");
  gl.shader_manager.load("fxaa", "shaders/fxaa");
  gl.shader_manager.load("ssao", "shaders/ssao");
  gl.shader_manager.load("bloom", "shaders/bloom");
  gl.shader_manager.load("fog", "shaders/fog");
  gl.shader_manager.load("directionlight", "shaders/directionlight");
  gl.shader_manager.load("ambientlight", "shaders/ambientlight");
  gl.shader_manager.load("pointshader", "shaders/pointshader");
  gl.shader_manager.load("debug", "shaders/debug");
  gl.shader_manager.load("basic", "shaders/basic");
  gl.shader_manager.load("color", "shaders/color");
  gl.shader_manager.load("fullscreen", "shaders/fullscreen");
  gl.shader_manager.load("lightShadow", "shaders/light-shadow");
  PROFILE_END();

  PROFILE("loading textures");
  loader.add_texture(std::string("default.dds"));
  loader.add_texture(std::string("spec.dds"));
  loader.add_texture(std::string("alpha.dds"));

  std::vector<const char*> faces;
  faces.push_back("right.dds");
  faces.push_back("left.dds");
  faces.push_back("top.dds");
  faces.push_back("bottom.dds");
  faces.push_back("back.dds");
  faces.push_back("front.dds");

  loader.add_cubemap("galaxySkybox", &faces);
  loader.load_mesh("sponza.obj", &gl);

  loader.start_loading();
  PROFILE_END();

  primitives.set_renderer(&gl);
  debug_draw.init();

  skybox.texture = loader.get("galaxySkybox");
  skybox.mesh = primitives.getCube();

  PROFILE("adding objects to scene");

  PROFILE_END();

  PROFILE("Scene init");
  scene.init(&loader, &gl);
  PROFILE_END();

  running = true;
  oldTime = SDL_GetTicks();

  PROFILE_END();
}

void Game::run() {
  while (running) {
    tick();
  }
}

void Game::exit() {
  PROFILER_EXPORT();
}

void Game::tick() {
  int currentTime = SDL_GetTicks();
  float diff = (currentTime - oldTime) / 1000.0f;
  oldTime = currentTime;

  frameCounter += 1;
  frameTime += diff;
  if (frameTime > 1.0f) {
    /* printf("fps: %d\n", frameCounter); */
    frameCounter = 0;
    frameTime = 0;
  }

  PROFILE("Frame");
  update(diff);
  render();
  PROFILE_END();

  SDL_GL_SwapWindow(window);
}

void Game::update(float time) {
  PROFILE("Update");
  totalTime += time;

  SDL_Event event;
  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
          SDL_SetRelativeMouseMode(SDL_TRUE);
        }
        break;
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEWHEEL:
        {
          float value = (float)event.wheel.y;
          light_radius += value;
          printf("light radius: %f\n", light_radius);
        } break;
      case SDL_MOUSEBUTTONDOWN:
        /* debug_draw.addLine(camera.position + camera.forward * 2.0f, camera.position + camera.forward * 1000.0f, 100); */
        {
          Light *light = new Light();
          light->type = kPoint;
          light->radius = 500.0f;
          light->position = camera.position;
          light->color = glm::vec3(((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX)
              );
          light->isCastingShadow = false;
          light->camera.position = light->position;
          light->camera.rotation = camera.rotation;
          light->camera.far = light->radius * 3;
          light->camera.update_matrix();
          /* light->frame_buffer.init(512, 512, true); */
          scene.lights.push_back(light);
        }
        break;
    }
  }

  SDL_PumpEvents();

  gl.anti_alias = !keyboard_state[SDL_SCANCODE_U];
  gl.ssao = keyboard_state[SDL_SCANCODE_Y];
  gl.fog = !keyboard_state[SDL_SCANCODE_T];
  gl.bloom = keyboard_state[SDL_SCANCODE_R];

  renderWireframe = keyboard_state[SDL_SCANCODE_P];

  int relX, relY;
  SDL_GetRelativeMouseState(&relX, &relY);
  camera.rotation[0] += relY/100.0f;
  camera.rotation[1] += relX/100.0f;

  static float halfpi = glm::half_pi<float>();

  if (camera.rotation[0] > halfpi - 0.001f) {
    camera.rotation[0] = halfpi - 0.001f;
  }

  if (camera.rotation[0] < -halfpi + 0.001f) {
    camera.rotation[0] = -halfpi + 0.001f;
  }

  camera.calculate_right_movement();

  float camera_speed = 15.0f;

  if (keyboard_state[SDL_SCANCODE_LSHIFT]) {
    camera_speed = 25.0f;
  }

  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    camera.position += glm::vec3(0.0, 1.0, 0.0) * camera_speed;
  }

  if (keyboard_state[SDL_SCANCODE_Q]) {
    camera.position += glm::vec3(0.0, -1.0, 0.0) * camera_speed;
  }

  if (keyboard_state[SDL_SCANCODE_W]) {
    camera.position += camera.forward * camera_speed;
  }

  if (keyboard_state[SDL_SCANCODE_S]) {
    camera.position -= camera.forward * camera_speed;
  }

  if (keyboard_state[SDL_SCANCODE_A]) {
    camera.position -= camera.right * camera_speed;
  }

  if (keyboard_state[SDL_SCANCODE_D]) {
    camera.position += camera.right * camera_speed;
  }

  camera.update_matrix();

  PROFILE_END();
}

void Game::render_from_camera(Camera *camera) {
  PROFILE("GBuffer");
  gl.gbuffer.bind_for_writing();
  glViewport(0, 0, gl.gbuffer.width, gl.gbuffer.height);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  gl.enable_depth_read();
  gl.enable_depth_write();

  glDisable(GL_BLEND);

  gl.clear(true);

  glClearStencil(0);
  glEnable(GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);

  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);

  gl.shader_manager.use("basic");
  PROFILE("Render entities");

  gl.shader_manager.current->set_uniform("uPMatrix", camera->view_matrix);

  Texture *texture_cache[8];

  int count = 0;

  for (auto it = scene.entities.begin(); it != scene.entities.end(); ++it) {
    Entity *entity = *it;

    Sphere sphere = entity->get_bounding_sphere();
    if (!camera->frustum.sphere_in_frustum(sphere.position, sphere.radius)) {
      continue;
    }
    count += 1;
    glm::mat4 model_view;

    model_view = glm::translate(model_view, entity->position);
    model_view = glm::rotate(model_view, entity->rotation.x, glm::vec3(1.0, 0.0, 0.0));
    model_view = glm::rotate(model_view, entity->rotation.y, glm::vec3(0.0, 1.0, 0.0));
    model_view = glm::rotate(model_view, entity->rotation.z, glm::vec3(0.0, 0.0, 1.0));
    model_view = glm::scale(model_view, entity->scale);

    gl.shader_manager.current->texture("uSampler", entity->texture->id, 0);
    gl.shader_manager.current->texture("uSpecular", entity->specularTexture->id, 1);
    gl.shader_manager.current->texture("uAlpha", entity->alpha_texture->id, 2);

    if (entity->normal_map != NULL) {
      gl.shader_manager.current->texture("uNormalMap", entity->normal_map->id, 3);
      gl.shader_manager.current->set_uniform("hasNormalMap", 1);
    } else {
      gl.shader_manager.current->set_uniform("hasNormalMap", 0);
    }

    glm::mat3 normal = glm::inverseTranspose(glm::mat3(model_view));
    gl.shader_manager.current->set_uniform("uNMatrix", normal);
    gl.shader_manager.current->set_uniform("uMVMatrix", model_view);

    gl.bind_mesh(entity->mesh);
    gl.draw(renderWireframe);
  }

  /* printf("%d\n", count); */
  PROFILE_END();
  gl.shader_manager.current->disable();

  gl.disable_depth_write();
  gl.disable_depth_read();
  glDisable(GL_CULL_FACE);
  PROFILE_END();
}

void Game::render() {
  PROFILE("Render");

  Camera *current_camera = &camera;

  gl.shader_manager.use("lightShadow");
  gl.enable_depth_write();
  gl.enable_depth_read();

  for (auto it_light = scene.lights.begin(); it_light != scene.lights.end(); it_light++) {
    Light *light = *it_light;

    if (light->isCastingShadow && !light->already_rendered) {
      light->frame_buffer.bind();
      gl.clear(true);
      glViewport(0, 0, light->frame_buffer.width, light->frame_buffer.height);
      gl.shader_manager.current->set_uniform("uPMatrix", light->camera.view_matrix);

      for (auto it = scene.entities.begin(); it != scene.entities.end(); it++) {
        Entity *entity = *it;


        glm::mat4 model_view;

        model_view = glm::translate(model_view, entity->position);
        model_view = glm::rotate(model_view, entity->rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model_view = glm::rotate(model_view, entity->rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model_view = glm::rotate(model_view, entity->rotation.z, glm::vec3(0.0, 0.0, 1.0));
        model_view = glm::scale(model_view, entity->scale);

        gl.shader_manager.current->set_uniform("uMVMatrix", model_view);

        glm::mat3 normal = glm::inverseTranspose(glm::mat3(model_view));
        gl.shader_manager.current->set_uniform("uNMatrix", normal);

        gl.bind_mesh(entity->mesh);
        gl.draw();
      }

      light->frame_buffer.unbind();

      if (!light->dynamic) {
        light->already_rendered = true;
      }
    }
  }

  gl.shader_manager.current->disable();

  render_from_camera(current_camera);
  gl.draw_lights(&scene.lights, primitives.get_sphere(), current_camera);

  gl.draw_skybox(&skybox, current_camera);

  debug_render(current_camera);

  gl.disable_depth_read();
  gl.final_render(current_camera);

  if (keyboard_state[SDL_SCANCODE_O]) {
    gl.debug_renderer_gbuffer(&gl.gbuffer);
  }

  PROFILE_END();
}

void Game::debug_render(Camera *camera) {
  gl.enable_depth_read();

  if (keyboard_state[SDL_SCANCODE_I]) {
    gl.shader_manager.use("color");

    gl.shader_manager.current->set_uniform("uPMatrix", camera->view_matrix);

    for (auto it = scene.entities.begin(); it != scene.entities.end(); it++) {
      Entity *entity = *it;

      Sphere sphere = entity->get_bounding_sphere();
      if (!camera->frustum.sphere_in_frustum(sphere.position, sphere.radius)) {
        continue;
      }

      gl.bind_mesh(primitives.get_sphere());

      glm::mat4 model_view;
      model_view = glm::translate(model_view, sphere.position);
      model_view = glm::scale(model_view, glm::vec3(sphere.radius));
      gl.shader_manager.current->set_uniform("color", glm::vec3(1.0, 1.0, 1.0));
      gl.shader_manager.current->set_uniform("uMVMatrix", model_view);

      gl.draw(true);
    }

    gl.shader_manager.current->disable();
  }

  gl.shader_manager.use("debug");
    gl.shader_manager.current->set_uniform("uPMatrix", camera->view_matrix);
    debug_draw.draw();
  gl.shader_manager.current->disable();
}
