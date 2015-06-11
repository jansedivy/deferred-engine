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

#include "profiler.h"

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
#include "error.h"
#include "scene.h"
#include "frame_buffer.h"
#include "sphere.h"

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
    void render_from_camera(Camera *camera);
    void debug_render(Camera *camera);

    bool running;

    float light_radius;

    SDL_Window *window;
    Primitives primitives;
    Renderer gl;
    Camera camera;

    Loader loader;
    const Uint8 *keyboard_state;

    float totalTime = 0.0f;

    Texture *skyboxTexture;
    bool renderWireframe;

    Skybox skybox;
    DebugDraw debug_draw;

    int width;
    int height;

    Scene scene;

  private:
    int oldTime = 0;

    int frameCounter = 0;
    float frameTime = 0.0f;
};
