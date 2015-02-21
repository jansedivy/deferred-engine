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

#include <map>

class Loader {
  public:
    Texture *loadTexture(const char *path);
    Texture *loadCubeMap(std::vector<const char*> *faces);

    void addTexture(const char *path);
    void addCubemap(const char *name, std::vector<const char*> *faces);
    Texture* get(const char *path);

    void startLoading();

    void loadMesh(const char *path, std::vector<Mesh*> *meshes, Renderer *renderer);
  private:
    unsigned char* loadImageData(const char *path, int *width, int *height);
    std::map<std::string, Texture*> textures;
    void loadImagesInQueue();

    enum TextureType {
      kNormal,
      kCubemap
    };

    struct TextureLoading {
      const char *path;
      std::vector<const char*> *paths;

      const char *name;
      TextureType type;
    };

    std::vector<TextureLoading> texturesToLoad;
};
