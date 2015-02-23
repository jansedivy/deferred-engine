#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "mesh.h"
#include "skybox.h"
#include "camera.h"
#include "gbuffer.h"
#include "shader_manager.h"

class Renderer {
  public:
    Renderer() {};
    void init(int w, int h);

    void draw(bool wireframe=false);
    void debugRendererGBuffer(GBuffer *framebuffer, Mesh *fullscreenMesh);
    void renderFullscreenTexture(GLuint texture, Mesh *fullscreenMesh);

    void useMesh(Mesh *mesh);
    void populateBuffers(Mesh *mesh);
    Shader *shader;
    void drawSkybox(Skybox *skybox, Camera *camera);
    ShaderManager shaderManager;

    int width;
    int height;
  private:
    Mesh *currentMesh;
};


