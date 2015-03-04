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
#include "common.h"
#include "frame_buffer.h"

class Renderer {
  public:
    Renderer() {};
    void init(int w, int h);

    void draw(bool wireframe=false);
    void debugRendererGBuffer(GBuffer *framebuffer);
    void renderFullscreenTexture(GLuint texture);

    void drawLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera);

    void renderDirectionalLights(std::vector<Light> *lights, Profiler *profiler, Camera *camera);
    void renderPointLights(std::vector<Light> *lights, Profiler *profiler, Mesh *sphere, Camera *camera);
    void renderAmbientLight(std::vector<Light> *lights, Profiler *profiler, Camera *camera);

    void finalRender(Profiler *profiler, Camera *camera);

    void bindMesh(Mesh *mesh);
    void populateBuffers(Mesh *mesh);

    void clear(bool clearDepth=false);
    void enableDepthRead();
    void disableDepthRead();

    void enableDepthWrite();
    void disableDepthWrite();

    void bindScreenAlignedQuad();
    void drawScreenAlignedQuad();

    void drawSkybox(Skybox *skybox, Camera *camera);
    ShaderManager shaderManager;

    GBuffer gbuffer;

    int kernelSize;
    glm::vec3 *kernel;

    int noiseSize;
    glm::vec3 noise[16];

    GLuint noiseId;

    int width;
    int height;

    bool antiAlias;
    bool ssao;
    float ssaoRadius;

    bool fog;
    bool bloom;

    GLuint screenAlignedQuad;

  private:
    Mesh *currentMesh;
};
