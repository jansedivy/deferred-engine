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
  std::string textureName;
  std::string normalName;
  std::string specularName;
  std::string alphaName;
};

class Loader {
  public:
    Texture* loadTexture(TextureLoading textureLoading);
    Texture *loadCubeMap(std::vector<const char*> *faces);

    void addTexture(std::string path);
    void addCubemap(const char *name, std::vector<const char*> *faces);
    Texture* get(const char *path);

    void startLoading();

    void loadMesh(const char *path, std::vector<LoadedMesh> *meshes, Renderer *gl);
  private:
    unsigned char* loadImageData(const char *path, int *width, int *height);
    std::map<std::string, Texture*> textures;
    void loadImagesInQueue();

    std::vector<TextureLoading> texturesToLoad;
};
