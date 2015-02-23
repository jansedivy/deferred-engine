#include "game.h"

Game::Game() {
  width = 1280;
  height = (float)width * 9.0f/16.0f;

  SDL_Init(SDL_INIT_EVERYTHING);

  char *base_path = SDL_GetBasePath();
  chdir(base_path);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

  keyboardState = SDL_GetKeyboardState(NULL);

  SDL_GLContext context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(-1);

  renderer.init(width, height);
  gbuffer.init(width, height);

  camera.setAspect((float)width/(float)height);
  camera.position[0] -= 20.0f;
  camera.position[1] += 30.0f;
  camera.position[2] -= 20.0f;

  profile.start("loading shaders");
  renderer.shaderManager.load("skybox", "skybox");
  renderer.shaderManager.load("directionlight", "directionlight");
  renderer.shaderManager.load("pointshader", "pointshader");
  renderer.shaderManager.load("debug", "debug");
  renderer.shaderManager.load("basic", "basic");
  renderer.shaderManager.load("color", "color");
  renderer.shaderManager.load("fullscreen", "fullscreen");
  profile.end();

  profile.start("loading textures");
  loader.addTexture("AM3.jpg");
  loader.addTexture("img.png");
  loader.addTexture("planet.png");

  std::vector<const char*> faces;
  faces.push_back("right.png");
  faces.push_back("left.png");
  faces.push_back("top.png");
  faces.push_back("bottom.png");
  faces.push_back("back.png");
  faces.push_back("front.png");

  loader.addCubemap("galaxySkybox", &faces);

  loader.startLoading();
  profile.end();

  primitives.renderer = &renderer;

  debugDraw.init();

  fullscreenMesh = primitives.getQuad();

  skybox.texture = loader.get("galaxySkybox");
  skybox.mesh = primitives.getCube();

  Texture *asteroidTexture = loader.get("AM3.jpg");
  Texture *texture = loader.get("img.png");
  Texture *planetTexture = loader.get("planet.png");

  profile.start("loading meshes");
  std::vector<Mesh*> meshes;
  loader.loadMesh("mesh.obj", &meshes, &renderer);

  std::vector<Mesh*> sponza;
  loader.loadMesh("sponza.obj", &sponza, &renderer);
  profile.end();

  profile.start("adding objects to scene");
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
      light.direction = glm::normalize(glm::vec3(1.0, -0.2, 0.4));
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

  int xSize = 50;
  int ySize = 50;
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

  profile.end();

  running = true;
  oldTime = SDL_GetTicks();
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

  profile.start("Frame");
  update(diff);
  render();
  profile.end();
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

        // TODO(sedivy): move to destructor?
        profile.exportData();
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

  profile.start("Update");
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
  /* printf("%f, %f, %f\n", camera.position.x, camera.position.y, camera.position.z); */

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
  profile.end();
}

void Game::renderFromCamera(Camera camera) {
  gbuffer.bindForWriting();
  glViewport(0, 0, gbuffer.width, gbuffer.height);

  glDisable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glDisable(GL_BLEND);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderer.shader = renderer.shaderManager.get("basic");
  renderer.shader->use();

  profile.start("Render entities");
  for (auto it = entities.begin(); it != entities.end(); ++it) {
    /* float radius = it->getBoundingRadius(); */
    /* if (!camera.frustum.sphereInFrustum(it->position, radius)) { */
    /*   break; */
    /* } */

    glm::mat4 modelView;

    modelView = glm::translate(modelView, it->position);
    modelView = glm::rotate(modelView, it->rotation.x, glm::vec3(1.0, 0.0, 0.0));
    modelView = glm::rotate(modelView, it->rotation.y, glm::vec3(0.0, 1.0, 0.0));
    modelView = glm::rotate(modelView, it->rotation.z, glm::vec3(0.0, 0.0, 1.0));
    modelView = glm::scale(modelView, it->scale);

    glm::mat3 normal = glm::inverseTranspose(glm::mat3(modelView));

    renderer.shader->texture("uSampler", it->texture->id, 0);
    renderer.shader->mat3("uNMatrix", normal);
    renderer.shader->mat4("uPMatrix", camera.viewMatrix);
    renderer.shader->mat4("uMVMatrix", modelView);

    renderer.useMesh(it->mesh);
    renderer.draw(renderWireframe);
  }
  profile.end();

  renderer.shader->disable();

  renderer.shader = renderer.shaderManager.get("debug");
  renderer.shader->use();
    renderer.shader->mat4("uPMatrix", camera.viewMatrix);
    debugDraw.draw();
  renderer.shader->disable();

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
}

void Game::render() {
  profile.start("Render");
  // renderer.drawSkybox(&skybox, &camera);
  profile.start("GBuffer");
  renderFromCamera(camera);
  profile.end();

  profile.start("Lights");
  renderLights();
  profile.end();

  gbuffer.bindForReading();
  renderer.renderFullscreenTexture(gbuffer.finalTexture, fullscreenMesh);

  profile.end();

  if (keyboardState[SDL_SCANCODE_O]) {
    renderer.debugRendererGBuffer(&gbuffer, fullscreenMesh);
  }

  SDL_GL_SwapWindow(window);
}

void Game::renderLights() {
  gbuffer.bindForLight();

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_STENCIL_TEST);
  renderPointLights();
  glDisable(GL_STENCIL_TEST);

  renderDirectionalLights();

  glDisable(GL_BLEND);
}

void Game::renderDirectionalLights() {
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  renderer.shader = renderer.shaderManager.get("directionlight");
  renderer.shader->use();

  gbuffer.bindForLight();

  renderer.shader->texture("diffuseTexture", gbuffer.texture, 0);
  renderer.shader->texture("normalTexture", gbuffer.normalTexture, 1);
  renderer.shader->texture("positionTexture", gbuffer.positionTexture, 2);

  renderer.useMesh(fullscreenMesh);

  for (auto it = lights.begin(); it != lights.end(); it++) {
    if (it->type == kDirectional) {
      renderer.shader->vec3("lightColor", it->color);
      renderer.shader->vec3("lightDirection", it->direction);
      renderer.draw();
    }
  }

  renderer.shader->disable();
};

void Game::renderPointLight(Light *light) {
  renderer.shader = renderer.shaderManager.get("pointshader");
  renderer.shader->use();

  glDrawBuffer(GL_COLOR_ATTACHMENT4);

  glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  renderer.useMesh(primitives.getSphere());
  glm::mat4 modelView;
  modelView = glm::translate(modelView, light->position);
  modelView = glm::scale(modelView, glm::vec3(light->radius));
  glm::mat4 invProjection = glm::inverse(camera.viewMatrix);

  renderer.shader->texture("diffuseTexture", gbuffer.texture, 0);
  renderer.shader->texture("normalTexture", gbuffer.normalTexture, 1);
  renderer.shader->texture("positionTexture", gbuffer.positionTexture, 2);
  renderer.shader->texture("depthTexture", gbuffer.depthTexture, 3);
  renderer.shader->mat4("uPMatrix", camera.viewMatrix);
  renderer.shader->mat4("invProjection", invProjection);
  renderer.shader->mat4("uMVMatrix", modelView);
  renderer.shader->vec3("lightPosition", light->position);
  renderer.shader->fl("lightRadius", light->radius);
  renderer.shader->vec3("lightColor", light->color);
  renderer.draw();

  renderer.shader->disable();

  glDisable(GL_BLEND);
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
}

void Game::setupStenciForLight(Light *light) {
  glDrawBuffer(GL_NONE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glStencilFunc(GL_ALWAYS, 0, 0);
  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

  glClear(GL_STENCIL_BUFFER_BIT);

  renderer.shader = renderer.shaderManager.get("color");
  renderer.shader->use();

    renderer.shader->mat4("uPMatrix", camera.viewMatrix);

    renderer.useMesh(primitives.getSphere());

    glm::mat4 modelView;
    modelView = glm::translate(modelView, light->position);
    modelView = glm::scale(modelView, glm::vec3(light->radius));
    renderer.shader->mat4("uMVMatrix", modelView);

    renderer.draw();

  renderer.shader->disable();
}

void Game::renderPointLights() {
  profile.start("Point Lights");
  gbuffer.bindForLight();
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  for (auto it = lights.begin(); it != lights.end(); it++) {
    if (it->type == kPoint) {
      Light light = *it;
      setupStenciForLight(&light);
      renderPointLight(&light);
    }
  }
  profile.end();
}
