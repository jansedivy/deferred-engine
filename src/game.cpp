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
  /* loader.addTexture("AM3.jpg"); */
  loader.addTexture(std::string("img.dds"));
  loader.addTexture(std::string("planet.dds"));
  /* loader.addTexture("marble1.jpg"); */
  loader.addTexture("default.dds");
  loader.addTexture(std::string("normal.jpg"));
  loader.addTexture(std::string("spec.dds"));
  loader.addTexture(std::string("bricks.jpg"));

  profiler.start("loading meshes");
  std::vector<LoadedMesh> meshes;
  loader.loadMesh("mesh.obj", &meshes, &gl);

  std::vector<LoadedMesh> sponza;
  loader.loadMesh("sponza.obj", &sponza, &gl);

  profiler.end();

  std::vector<const char*> faces;
  faces.push_back("right.dds");
  faces.push_back("left.dds");
  faces.push_back("top.dds");
  faces.push_back("bottom.dds");
  faces.push_back("back.dds");
  faces.push_back("front.dds");

  loader.addCubemap("galaxySkybox", &faces);

  loader.startLoading();
  profiler.end();

  primitives.setRenderer(&gl);

  debugDraw.init();

  skybox.texture = loader.get("galaxySkybox");
  skybox.mesh = primitives.getCube();

  Texture *asteroidTexture = loader.get("bricks.jpg");
  Texture *texture = loader.get("img.dds");
  Texture *planetTexture = loader.get("planet.dds");

  profiler.start("adding objects to scene");
  for (auto it = sponza.begin(); it != sponza.end(); it++) {
    Entity entity;
    if (!it->textureName.empty()) {
      entity.texture = loader.get(it->textureName.c_str());
    } else {
      entity.texture = loader.get("default.dds");
    }

    entity.normalMap = NULL;
    entity.specularTexture = NULL;

    if (!it->normalName.empty()) {
      entity.normalMap = loader.get(it->normalName.c_str());
    }

    if (!it->specularName.empty()) {
      entity.specularTexture = loader.get(it->specularName.c_str());
    } else {
      entity.specularTexture = loader.get("spec.dds");
    }

    entity.type = kOther;
    entity.scale = glm::vec3(1.0, 1.0, 1.0);
    entity.mesh = it->mesh;
    entity.rotation = glm::vec3(0.0, 0.0, 0.0);
    entity.position = glm::vec3(-2000.0, 0.0, -1000.0);
    entities.push_back(entity);
  }

  {
    Light light;
    light.type = kAmbient;
    light.color = glm::vec3(0.1, 0.1, 0.1);
    lights.push_back(light);
  }

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
  int current_time = SDL_GetTicks();
  float diff = (current_time - oldTime) / 1000.0f;
  oldTime = current_time;

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
          if (keyboardState[SDL_SCANCODE_LSHIFT]) {
            light.color = glm::vec3(238.0/255.0, 230.0/255.0, 103.0/255.0);
          } else {
            light.color = glm::vec3(
                ((float)rand()/(float)RAND_MAX),
                ((float)rand()/(float)RAND_MAX),
                ((float)rand()/(float)RAND_MAX)
                );
          }
          lights.push_back(light);
        }

        break;
    }
  }

  SDL_PumpEvents();

  gl.antiAlias = !keyboardState[SDL_SCANCODE_U];
  gl.ssao = keyboardState[SDL_SCANCODE_Y];
  gl.fog = !keyboardState[SDL_SCANCODE_T];

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

  if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_E]) {
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

  Texture *normalTexture = loader.get("normal.jpg");

  int count = 0;

  for (auto it = entities.begin(); it != entities.end(); ++it) {
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

    if (it->texture) {
      gl.shaderManager.current->texture("uSampler", it->texture->id, 0);
    }

    gl.shaderManager.current->texture("uSpecular", it->specularTexture->id, 1);

    if (it->normalMap != NULL) {
      gl.shaderManager.current->texture("uNormalMap", it->normalMap->id, 2);
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

  renderFromCamera(&camera);
  gl.drawLights(&lights, &profiler, primitives.getSphere(), &camera);

  debugRender();

  gl.drawSkybox(&skybox, &camera);

  gl.disableDepthRead();
  gl.finalRender(&profiler, &camera);

  if (keyboardState[SDL_SCANCODE_O]) {
    gl.debugRendererGBuffer(&gl.gbuffer);
  }

  profiler.end();
}

void Game::debugRender() {
  gl.enableDepthRead();

  if (keyboardState[SDL_SCANCODE_I]) {
    gl.shaderManager.use("color");

    gl.shaderManager.current->setUniform("uPMatrix", camera.viewMatrix);

    for (auto it = lights.begin(); it != lights.end(); it++) {
      if (it->type == kPoint) {
        if (!camera.frustum.sphereInFrustum(it->position, it->radius)) {
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
    gl.shaderManager.current->setUniform("uPMatrix", camera.viewMatrix);
    debugDraw.draw();
  gl.shaderManager.current->disable();
}
