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
    void debug_renderer_gbuffer(GBuffer *framebuffer);
    void render_fullscreen_texture(GLuint texture);

    void draw_lights(std::vector<Light*> *lights, Mesh *sphere, Camera *camera);

    void render_directional_lights(std::vector<Light*> *lights, Camera *camera);
    void render_point_lights(std::vector<Light*> *lights, Mesh *sphere, Camera *camera);
    void render_ambient_light(std::vector<Light*> *lights, Camera *camera);

    void final_render(Camera *camera);

    void bind_mesh(Mesh *mesh);
    void populate_buffers(Mesh *mesh);

    void clear(bool clear_depth=false);
    void enable_depth_read();
    void disable_depth_read();

    void enable_depth_write();
    void disable_depth_write();

    void bind_screen_aligned_quad();
    void draw_screen_aligned_quad();

    void draw_skybox(Skybox *skybox, Camera *camera);
    ShaderManager shader_manager;

    GBuffer gbuffer;

    int kernel_size;
    glm::vec3 *kernel;

    int noise_size;
    glm::vec3 noise[16];

    GLuint noise_id;

    int width;
    int height;

    bool anti_alias;
    bool ssao;
    float ssao_radius;

    bool fog;
    bool bloom;

    GLuint screen_aligned_quad;

  private:
    Mesh *current_mesh;
};
