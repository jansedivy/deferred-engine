#include "game.h"

void Game::init() {
  profiler.start("Game init");
  lightRadius = 500.0f;
  width = 1280;
  height = (float)width * 9.0f/16.0f;

  profiler.start("SDL load");
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

  keyboardState = SDL_GetKeyboardState(NULL);

  SDL_GLContext context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(-1);
  profiler.end();

  gl.init(width, height);

  camera.setAspect((float)width/(float)height);
  camera.position[0] -= 20.0f;
  camera.position[1] += 30.0f;
  camera.position[2] -= 20.0f;

  profiler.start("loading shaders");
  gl.shaderManager.load("skybox", "shaders/skybox");
  gl.shaderManager.load("fxaa", "shaders/fxaa");
  gl.shaderManager.load("ssao", "shaders/ssao");
  gl.shaderManager.load("bloom", "shaders/bloom");
  gl.shaderManager.load("fog", "shaders/fog");
  gl.shaderManager.load("directionlight", "shaders/directionlight");
  gl.shaderManager.load("ambientlight", "shaders/ambientlight");
  gl.shaderManager.load("pointshader", "shaders/pointshader");
  gl.shaderManager.load("debug", "shaders/debug");
  gl.shaderManager.load("basic", "shaders/basic");
  gl.shaderManager.load("color", "shaders/color");
  gl.shaderManager.load("fullscreen", "shaders/fullscreen");
  profiler.end();

  profiler.start("loading textures");
  loader.addTexture(std::string("default.dds"));
  loader.addTexture(std::string("spec.dds"));
  loader.addTexture(std::string("alpha.dds"));

  std::vector<const char*> faces;
  faces.push_back("right.dds");
  faces.push_back("left.dds");
  faces.push_back("top.dds");
  faces.push_back("bottom.dds");
  faces.push_back("back.dds");
  faces.push_back("front.dds");

  loader.addCubemap("galaxySkybox", &faces);
  loader.loadMesh("sponza.obj", &gl);

  loader.startLoading();
  profiler.end();

  primitives.setRenderer(&gl);
  debugDraw.init();

  skybox.texture = loader.get("galaxySkybox");
  skybox.mesh = primitives.getCube();

  profiler.start("adding objects to scene");

  profiler.end();

  profiler.start("Scene init");
  scene.init(&loader, &gl);
  profiler.end();

  running = true;
  oldTime = SDL_GetTicks();

  profiler.end();
}

void Game::run() {
  while (running) {
    tick();
  }
}

void Game::exit() {
  profiler.exportData();
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

  profiler.start("Frame");
  update(diff);
  render();
  profiler.end();

  SDL_GL_SwapWindow(window);
}

void Game::update(float time) {
  profiler.start("Update");
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
          lightRadius += value;
          printf("light radius: %f\n", lightRadius);
        } break;
      case SDL_MOUSEBUTTONDOWN:
        /* debugDraw.addLine(camera.position + camera.forward * 2.0f, camera.position + camera.forward * 1000.0f, 100); */
        {
          Light light;
          light.type = kPoint;
          light.radius = lightRadius;
          light.position = camera.position;
          light.color = glm::vec3(((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX)
              );
          light.isCastingShadow = false;
          light.camera.position = light.position;
          light.camera.updateMatrix();
          light.frameBuffer.init(512, 512);
          scene.lights.push_back(light);
        }

        break;
    }
  }

  SDL_PumpEvents();

  gl.antiAlias = !keyboardState[SDL_SCANCODE_U];
  gl.ssao = keyboardState[SDL_SCANCODE_Y];
  gl.fog = !keyboardState[SDL_SCANCODE_T];
  gl.bloom = keyboardState[SDL_SCANCODE_R];

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

  camera.calculateRightMovement();

  float cameraSpeed = 15.0f;

  if (keyboardState[SDL_SCANCODE_LSHIFT]) {
    cameraSpeed = 25.0f;
  }

  if (keyboardState[SDL_SCANCODE_SPACE]) {
    camera.position += glm::vec3(0.0, 1.0, 0.0) * cameraSpeed;
  }

  renderWireframe = keyboardState[SDL_SCANCODE_P];

  if (keyboardState[SDL_SCANCODE_Q]) {
    camera.position += glm::vec3(0.0, -1.0, 0.0) * cameraSpeed;
  }

  if (keyboardState[SDL_SCANCODE_W]) {
    camera.position += camera.forward * cameraSpeed;
  }

  if (keyboardState[SDL_SCANCODE_S]) {
    camera.position -= camera.forward * cameraSpeed;
  }

  if (keyboardState[SDL_SCANCODE_A]) {
    camera.position -= camera.right * cameraSpeed;
  }

  if (keyboardState[SDL_SCANCODE_D]) {
    camera.position += camera.right * cameraSpeed;
  }

  camera.updateMatrix();

  profiler.end();
}

void Game::renderFromCamera(Camera *camera) {
  profiler.start("GBuffer");
  gl.gbuffer.bindForWriting();
  glViewport(0, 0, gl.gbuffer.width, gl.gbuffer.height);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  gl.enableDepthRead();
  gl.enableDepthWrite();

  glDisable(GL_BLEND);

  gl.clear(true);

  glClearStencil(0);
  glEnable(GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);

  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);

  gl.shaderManager.use("basic");
  profiler.start("Render entities");

  gl.shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

  Texture *textureCache[8];

  int count = 0;

  for (auto it = scene.entities.begin(); it != scene.entities.end(); ++it) {
    float radius = it->getBoundingRadius();
    if (!camera->frustum.sphereInFrustum(it->position, radius)) {
      continue;
    }
    count += 1;
    glm::mat4 modelView;

    modelView = glm::translate(modelView, it->position);
    modelView = glm::rotate(modelView, it->rotation.x, glm::vec3(1.0, 0.0, 0.0));
    modelView = glm::rotate(modelView, it->rotation.y, glm::vec3(0.0, 1.0, 0.0));
    modelView = glm::rotate(modelView, it->rotation.z, glm::vec3(0.0, 0.0, 1.0));
    modelView = glm::scale(modelView, it->scale);

    gl.shaderManager.current->texture("uSampler", it->texture->id, 0);
    gl.shaderManager.current->texture("uSpecular", it->specularTexture->id, 1);
    gl.shaderManager.current->texture("uAlpha", it->alphaTexture->id, 2);

    if (it->normalMap != NULL) {
      gl.shaderManager.current->texture("uNormalMap", it->normalMap->id, 3);
      gl.shaderManager.current->setUniform("hasNormalMap", 1);
    } else {
      gl.shaderManager.current->setUniform("hasNormalMap", 0);
    }

    glm::mat3 normal = glm::inverseTranspose(glm::mat3(modelView));
    gl.shaderManager.current->setUniform("uNMatrix", normal);
    gl.shaderManager.current->setUniform("uMVMatrix", modelView);

    gl.bindMesh(it->mesh);
    gl.draw(renderWireframe);
  }

  /* printf("%d\n", count); */
  profiler.end();
  gl.shaderManager.current->disable();

  gl.disableDepthWrite();
  gl.disableDepthRead();
  glDisable(GL_CULL_FACE);
  profiler.end();
}

void Game::render() {
  profiler.start("Render");

  Camera *currentCamera = &camera;

  gl.shaderManager.use("color");
  gl.enableDepthWrite();
  gl.enableDepthRead();

  for (auto light = scene.lights.begin(); light != scene.lights.end(); light++) {
    if (light->isCastingShadow) {
      light->frameBuffer.bind();
      gl.clear(true);
      glViewport(0, 0, light->frameBuffer.width, light->frameBuffer.height);
      gl.shaderManager.current->setUniform("uPMatrix", light->camera.viewMatrix);

      for (auto it = scene.entities.begin(); it != scene.entities.end(); it++) {
        glm::mat4 modelView;

        modelView = glm::translate(modelView, it->position);
        modelView = glm::rotate(modelView, it->rotation.x, glm::vec3(1.0, 0.0, 0.0));
        modelView = glm::rotate(modelView, it->rotation.y, glm::vec3(0.0, 1.0, 0.0));
        modelView = glm::rotate(modelView, it->rotation.z, glm::vec3(0.0, 0.0, 1.0));
        modelView = glm::scale(modelView, it->scale);

        gl.shaderManager.current->setUniform("uMVMatrix", modelView);

        gl.bindMesh(it->mesh);
        gl.draw();
      }

      light->frameBuffer.unbind();
    }
  }

  gl.shaderManager.current->disable();

  renderFromCamera(currentCamera);
  gl.drawLights(&scene.lights, &profiler, primitives.getSphere(), currentCamera);

  gl.drawSkybox(&skybox, currentCamera);

  debugRender(currentCamera);

  gl.disableDepthRead();
  gl.finalRender(&profiler, currentCamera);

  if (keyboardState[SDL_SCANCODE_O]) {
    gl.debugRendererGBuffer(&gl.gbuffer);
  }

  profiler.end();
}

void Game::debugRender(Camera *camera) {
  gl.enableDepthRead();

  if (keyboardState[SDL_SCANCODE_I]) {
    gl.shaderManager.use("color");

    gl.shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

    for (auto it = scene.lights.begin(); it != scene.lights.end(); it++) {
      if (it->type == kPoint) {
        if (!camera->frustum.sphereInFrustum(it->position, it->radius)) {
          continue;
        }

        gl.bindMesh(primitives.getSphere());

        glm::mat4 modelView;
        modelView = glm::translate(modelView, it->position);
        modelView = glm::scale(modelView, glm::vec3(it->radius));
        gl.shaderManager.current->setUniform("uMVMatrix", modelView);

        gl.draw(true);
      }
    }

    gl.shaderManager.current->disable();
  }

  gl.shaderManager.use("debug");
    gl.shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);
    debugDraw.draw();
  gl.shaderManager.current->disable();
}
