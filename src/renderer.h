#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "mesh.h"
#include "skybox.h"
#include "camera.h"
#include "framebuffer.h"

class Renderer {
  public:
    Renderer() {};
    void init(int w, int h);

    void draw(bool wireframe=false);
    void debugRenderFrameBuffer(FrameBuffer *framebuffer, Mesh *fullscreenMesh);

    void useMesh(Mesh *mesh);
    void populateBuffers(Mesh *mesh);
    Shader *shader;
    void drawSkybox(Skybox *skybox, Camera *camera);

    Shader normalShader;
    Shader directionShader;
    Shader pointShader;
    Shader fullscreen;
    Shader skyboxShader;
    Shader debugShader;

    int width;
    int height;
  private:
    Mesh *currentMesh;
};


