#pragma once

#include <GL/glew.h>

#include <SDL.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <vector>

#include "shader.h"
#include "renderer.h"
#include "camera.h"
#include "entity.h"
#include "loader.h"
#include "gbuffer.h"
#include "primitives.h"
#include "texture.h"
#include "skybox.h"
#include "mesh.h"
#include "debug_draw.h"
#include "light.h"
#include "profiler.h"
#include "error.h"

#include "file_io.h"

#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#endif

class Game {
  public:
    void init();
    void exit();
    void run();
    void tick();
    void update(float time);

    void render();
    void renderFromCamera(Camera *camera);
    void renderDirectionalLights();
    void renderPointLight(Light *light);
    void debugRender();

    bool running;

    float lightRadius;

    SDL_Window *window;
    Primitives primitives;
    Renderer gl;
    Camera camera;

    Loader loader;
    const Uint8 *keyboardState;

    std::vector<Light> lights;
    std::vector<Entity> entities;

    float totalTime = 0.0f;

    Texture *skyboxTexture;
    bool renderWireframe;

    Skybox skybox;
    DebugDraw debugDraw;
    Profiler profiler;

    int width;
    int height;

  private:
    unsigned int numbers = 0;
    double average = 0.0f;

    int oldTime = 0;
    int current_time = 0;

    int frameCounter = 0;
    float frameTime = 0.0f;
};
