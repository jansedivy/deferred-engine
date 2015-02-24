#include "loader.h"

Texture* Loader::get(const char *path) {
  if (textures.count(path)) {
    return textures[path];
  } else {
    showError("Error getting texture", "Undefined texture %s", path);
    return NULL;
  }
}

void Loader::addTexture(const char *path) {
  TextureLoading loading;
  loading.path = path;
  loading.name = path;
  loading.type = kNormal;

  texturesToLoad.push_back(loading);
}

void Loader::addCubemap(const char *name, std::vector<const char*> *faces) {
  textures[name] = loadCubeMap(faces);
  /* TextureLoading loading; */
  /* loading.paths = faces; */
  /* loading.name = name; */
  /* loading.type = kCubemap; */

  /* texturesToLoad.push_back(loading); */
}

void Loader::startLoading() {
  // TODO(sedivy): load in parallel
  loadImagesInQueue();
}

void Loader::loadImagesInQueue() {
  for (auto it = texturesToLoad.begin(); it != texturesToLoad.end(); it++) {
    switch (it->type) {
      case kNormal:
        textures[it->name] = loadTexture(it->path);
        break;
      case kCubemap:
        textures[it->name] = loadCubeMap(it->paths);
        break;
    }
  }
}

Texture* Loader::loadTexture(const char *path) {
  GLuint texture;

  int width;
  int height;

  unsigned char *image = loadImageData(path, &width, &height);
  if (!image) {
    showError("Error loading texture", "Error loading texture %s", path);
    return NULL;
  }

  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  /* glGenerateMipmap(GL_TEXTURE_2D); */

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  SOIL_free_image_data(image);

  Texture *result = new Texture();

  result->id = texture;
  result->width = width;
  result->height = height;

  return result;
}

Texture* Loader::loadCubeMap(std::vector<const char*> *faces) {
  Texture *result = new Texture();

  glGenTextures(1, &result->id);
  glActiveTexture(GL_TEXTURE0);

  int width,height;
  unsigned char* image;

  glBindTexture(GL_TEXTURE_CUBE_MAP, result->id);

  GLuint number = 0;
  for (auto it = faces->begin(); it != faces->end(); it++) {
    image = loadImageData(*it, &width, &height);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + number, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    number += 1;
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return result;
}

unsigned char* Loader::loadImageData(const char *path, int *width, int *height) {
  printf("Loading texture: %s\n", path);
  return SOIL_load_image(path, width, height, 0, SOIL_LOAD_RGB);
}

void Loader::loadMesh(const char *path, std::vector<Mesh*> *meshes, Renderer *gl) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals);

  if (!scene) {
    printf("Error: %s\n", importer.GetErrorString());
  }

  if (scene->HasMeshes()) {
    for (int i=0; i<scene->mNumMeshes; i++) {
      aiMesh *meshData = scene->mMeshes[i];

      float radius = 0.0f;
      Mesh *mesh = new Mesh();

      for (int l=0; l<meshData->mNumVertices; l++) {
        mesh->vertices.push_back(meshData->mVertices[l].x);
        mesh->vertices.push_back(meshData->mVertices[l].y);
        mesh->vertices.push_back(meshData->mVertices[l].z);

        radius = fmax(radius, abs(meshData->mVertices[l].x));
        radius = fmax(radius, abs(meshData->mVertices[l].y));
        radius = fmax(radius, abs(meshData->mVertices[l].z));

        mesh->normals.push_back(meshData->mNormals[l].x);
        mesh->normals.push_back(meshData->mNormals[l].y);
        mesh->normals.push_back(meshData->mNormals[l].z);

        if (meshData->mTextureCoords[0]) {
          mesh->uv.push_back(meshData->mTextureCoords[0][l].x);
          mesh->uv.push_back(meshData->mTextureCoords[0][l].y);
        }
      }

      for (int l=0; l<meshData->mNumFaces; l++) {
        aiFace face = meshData->mFaces[l];

        for (int j=0; j<face.mNumIndices; j++) {
          mesh->indices.push_back(face.mIndices[j]);
        }
      }

      gl->populateBuffers(mesh);

      mesh->boundingRadius = radius;

      meshes->push_back(mesh);
    }
  }
}
