#include "game.h"

void Game::init() {
  profiler.start("Game init");
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
  gl.shaderManager.load("directionlight", "shaders/directionlight");
  gl.shaderManager.load("pointshader", "shaders/pointshader");
  gl.shaderManager.load("debug", "shaders/debug");
  gl.shaderManager.load("basic", "shaders/basic");
  gl.shaderManager.load("color", "shaders/color");
  gl.shaderManager.load("fullscreen", "shaders/fullscreen");
  profiler.end();

  profiler.start("loading textures");
  /* loader.addTexture("AM3.jpg"); */
  loader.addTexture("img.png");
  loader.addTexture("planet.png");
  loader.addTexture("marble1.jpg");
  /* loader.addTexture("normal.jpg"); */
  /* loader.addTexture("bricks.jpg"); */

  /* std::vector<const char*> faces; */
  /* faces.push_back("right.png"); */
  /* faces.push_back("left.png"); */
  /* faces.push_back("top.png"); */
  /* faces.push_back("bottom.png"); */
  /* faces.push_back("back.png"); */
  /* faces.push_back("front.png"); */

  /* loader.addCubemap("galaxySkybox", &faces); */

  loader.startLoading();
  profiler.end();

  primitives.gl = &gl;

  debugDraw.init();

  fullscreenMesh = primitives.getQuad();

  /* skybox.texture = loader.get("galaxySkybox"); */
  /* skybox.mesh = primitives.getCube(); */

  Texture *asteroidTexture = loader.get("marble1.jpg");
  Texture *texture = loader.get("img.png");
  Texture *planetTexture = loader.get("planet.png");

  profiler.start("loading meshes");
  std::vector<Mesh*> meshes;
  loader.loadMesh("mesh.obj", &meshes, &gl);

  std::vector<Mesh*> sponza;
  loader.loadMesh("sponza.obj", &sponza, &gl);
  profiler.end();

  profiler.start("adding objects to scene");
  for (auto it = sponza.begin(); it != sponza.end(); it++) {
    Entity entity;
    entity.texture = asteroidTexture;
    entity.type = kOther;
    entity.scale = glm::vec3(1.0, 1.0, 1.0);
    entity.mesh = *it;
    entity.rotation = glm::vec3(0.0, 0.0, 0.0);
    entity.position = glm::vec3(-2000.0, 0.0, -1000.0);
    entities.push_back(entity);
  }

  {
    Light light;
    light.type = kDirectional;
    light.color = glm::vec3(0.2, 0.2, 0.2);
    light.direction = glm::vec3(0.0, 1.0, 0.0);
    lights.push_back(light);
  }

  {
    Light light;
    light.type = kDirectional;
    light.color = glm::vec3(0.2, 0.0, 0.0);
    light.direction = glm::vec3(0.0, -1.0, 0.0);
    lights.push_back(light);
  }

  {
    for (int i=0; i<10; i++) {
      Light light;
      light.type = kPoint;
      light.radius = 100.0f;
      light.position = glm::vec3(
          ((float)rand()/(float)RAND_MAX) * 500.0f - 250.0f,
          11.0,
          ((float)rand()/(float)RAND_MAX) * 500.0f - 250.0f
          );
      light.color = glm::vec3(
          ((float)rand()/(float)RAND_MAX),
          ((float)rand()/(float)RAND_MAX),
          ((float)rand()/(float)RAND_MAX)
          );
      lights.push_back(light);
    }
  }

  for (int i=0; i<50; i++) {
    Entity entity;
    entity.texture = asteroidTexture;
    entity.type = kAsteroid;
    entity.scale = glm::vec3(
        (((float)rand()/(float)RAND_MAX)/2.0 + 0.5) * 2.0f,
        (((float)rand()/(float)RAND_MAX)/2.0 + 0.5) * 2.0f,
        (((float)rand()/(float)RAND_MAX)/2.0 + 0.5) * 2.0f
        );
    entity.mesh = meshes[0];
    entity.x = 0;
    entity.y = 0;
    entity.rotation = glm::vec3(
        ((float)rand()/(float)RAND_MAX) * M_PI*2.0f,
        ((float)rand()/(float)RAND_MAX) * M_PI*2.0f,
        ((float)rand()/(float)RAND_MAX) * M_PI*2.0f
        );
    entity.position = glm::vec3(
        ((float)rand()/(float)RAND_MAX) * 8000.0f - 4000.0f,
        ((float)rand()/(float)RAND_MAX) * 8000.0f - 4000.0f,
        ((float)rand()/(float)RAND_MAX) * 8000.0f - 4000.0f
        );

    entities.push_back(entity);
  }

  {
    Entity entity;
    entity.texture = planetTexture;
    entity.scale = glm::vec3(2000.5, 2000.5, 2000.5);
    entity.mesh = primitives.getSphere();
    entity.x = 0;
    entity.y = 0;
    entity.type = kPlanet;
    entity.position = glm::vec3(3500.0, 500.0, 500.0);
    entities.push_back(entity);
  }

  {
    Entity entity;
    entity.texture = asteroidTexture;
    entity.scale = glm::vec3(200.0, 6.0, 200.0);
    entity.mesh = primitives.getCube();
    entity.x = 0;
    entity.y = 0;
    entity.type = kOther;
    entity.position = glm::vec3(200.0, 0.0, 0.0);
    entities.push_back(entity);
  }

  {
    Entity entity;
    entity.texture = planetTexture;
    entity.scale = glm::vec3(2000.5, 2000.5, 2000.5);
    entity.mesh = primitives.getSphere();
    entity.x = 0;
    entity.type = kPlanet;
    entity.y = 0;
    entity.position = glm::vec3(-5500.0, -3500.0, 500.0);
    entities.push_back(entity);
  }

  int xSize = 20;
  int ySize = 20;
  for (int x=0; x<xSize; x++) {
    for (int y=0; y<ySize; y++) {
      Entity entity;
      entity.texture = texture;
      entity.scale = glm::vec3(0.5, 0.5, 0.5);
      entity.mesh = primitives.getCube();
      entity.type = kBlock;
      entity.x = x-xSize/2;
      entity.y = y-ySize/2;
      entity.position = glm::vec3((x-xSize/2) * 1.5f, -4.0, (y-ySize/2) * 1.5f - 10.0f);
      entities.push_back(entity);
    }
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
}

void Game::update(float time) {
  totalTime += time;

  SDL_PumpEvents();
  SDL_Event event;
  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        /* debugDraw.addLine(camera.position + camera.forward * 2.0f, camera.position + camera.forward * 1000.0f, 100); */
        {
          Light light;
          light.type = kPoint;
          light.radius = 500.0f;
          light.position = camera.position;
          light.color = glm::vec3(
              ((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX),
              ((float)rand()/(float)RAND_MAX)
              );
          lights.push_back(light);
        }

        break;
    }
  }

  profiler.start("Update");
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

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    if (it->type == kPlanet) {
      it->rotation[1] += 0.02 * time;
    } else if (it->type == kAsteroid) {
    } else if (it->type == kBlock) {
      float position = cos(totalTime + it->x/10.0f);
      position += sin(totalTime + it->y/10.0f);
      it->position[1] = position * 10.0f;
    }
  }
  profiler.end();
}

void Game::renderFromCamera(Camera *camera) {
  profiler.start("GBuffer");
  glEnable(GL_STENCIL_TEST);

  gl.gbuffer.bindForWriting();
  glViewport(0, 0, gl.gbuffer.width, gl.gbuffer.height);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  gl.enableDepthRead();
  gl.enableDepthWrite();

  glDisable(GL_BLEND);

  gl.clear(true);

  gl.shaderManager.use("basic");
  profiler.start("Render entities");

  gl.shaderManager.current->setUniform("uPMatrix", camera->viewMatrix);

  Texture *textureCache[8];

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

    glm::mat3 normal = glm::inverseTranspose(glm::mat3(modelView));

    if (textureCache[0] != it->texture) {
      textureCache[0] = it->texture;
      gl.shaderManager.current->texture("uSampler", it->texture->id, 0);
    }

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
  gl.drawLights(&lights, &profiler, primitives.getSphere(), fullscreenMesh, &camera);
  profiler.end();

  debugRender();

  gl.disableDepthRead();

  gl.gbuffer.bindForReading();
  gl.renderFullscreenTexture(gl.gbuffer.finalTexture, fullscreenMesh);

  if (keyboardState[SDL_SCANCODE_O]) {
    gl.debugRendererGBuffer(&gl.gbuffer, fullscreenMesh);
  }

  SDL_GL_SwapWindow(window);
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
