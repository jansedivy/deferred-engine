#pragma once

#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture.h"
#include "mesh.h"
#include "renderer.h"
#include "error.h"

#include <map>

enum TextureType {
  kNormal,
  kCubemap
};

struct TextureLoading {
  std::string path;
  std::string name;

  TextureType type;

  /*       std::vector<const char*> *paths; */
};

struct LoadedMesh {
  Mesh *mesh;
  std::string texture_name;
  std::string normal_name;
  std::string specular_name;
  std::string alpha_name;
};

class Loader {
  public:
    Texture* loadTexture(TextureLoading textureLoading);
    Texture *loadCubeMap(std::vector<const char*> *faces);

    void add_texture(std::string path);
    void add_cubemap(const char *name, std::vector<const char*> *faces);
    void load_mesh(const char *path, Renderer *gl);

    Texture* get(const char *path);
    std::vector<LoadedMesh>* getModel(const char *path);

    void start_loading();

  private:
    unsigned char* loadImageData(const char *path, int *width, int *height);

    std::map<std::string, Texture*> textures;
    std::map<std::string, std::vector<LoadedMesh>*> models;

    void loadImagesInQueue();

    std::vector<TextureLoading> texturesToLoad;
};
