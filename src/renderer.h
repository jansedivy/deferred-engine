#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "mesh.h"
#include "skybox.h"
#include "camera.h"
#include "gbuffer.h"
#include "shader_manager.h"
#include "light.h"
#include "profiler.h"

class Renderer {
  public:
    Renderer() {};
    void init(int w, int h);

    void draw(bool wireframe=false);
    void debugRendererGBuffer(GBuffer *framebuffer, Mesh *fullscreenMesh);
    void renderFullscreenTexture(GLuint texture, Mesh *fullscreenMesh);

    void drawLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Mesh *fullscreenMesh, Camera *camera);

    void renderDirectionalLights(std::vector<Light> *lights, Profiler *profiler, Mesh *fullscreenMesh, Camera *camera);
    void renderPointLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera);

    void bindMesh(Mesh *mesh);
    void populateBuffers(Mesh *mesh);

    void clear(bool clearDepth=false);

    void drawSkybox(Skybox *skybox, Camera *camera);
    ShaderManager shaderManager;

    GBuffer gbuffer;

    int width;
    int height;
  private:
    Mesh *currentMesh;
};


