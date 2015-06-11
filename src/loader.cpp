#include "loader.h"

Texture* Loader::get(const char *path) {
  if (textures.count(path)) {
    return textures[path];
  } else {
    printf("undefined texture %s\n", path);
    /* showError("Error getting texture", "Undefined texture %s", path); */
    return NULL;
  }
}

std::vector<LoadedMesh>* Loader::getModel(const char *path) {
  if (models.count(path)) {
    return models[path];
  } else {
    printf("undefined model %s\n", path);
    return NULL;
  }
}

void Loader::add_texture(std::string path) {
  TextureLoading loading;
  loading.path = path;
  loading.name = path;
  loading.type = kNormal;

  texturesToLoad.push_back(loading);
}

void Loader::add_cubemap(const char *name, std::vector<const char*> *faces) {
  textures[name] = loadCubeMap(faces);
  /* TextureLoading loading; */
  /* loading.paths = faces; */
  /* loading.name = name; */
  /* loading.type = kCubemap; */

  /* texturesToLoad.push_back(loading); */
}

void Loader::start_loading() {
  // TODO(sedivy): load in parallel
  loadImagesInQueue();
}

void Loader::loadImagesInQueue() {
  for (auto it = texturesToLoad.begin(); it != texturesToLoad.end(); it++) {
    switch (it->type) {
      case kNormal:
        textures[it->name] = loadTexture(*it);
        break;
      /* case kCubemap: */
      /*   textures[it->name] = loadCubeMap(it->paths); */
      /*   break; */
    }
  }
}

Texture* Loader::loadTexture(TextureLoading textureLoading) {
  if (textures[textureLoading.name] != NULL) {
    return textures[textureLoading.name];
  }

  GLuint texture;

  int width;
  int height;

  unsigned char *image = loadImageData(textureLoading.path.c_str(), &width, &height);
  if (!image) {
    showError("Error loading texture", "Error loading texture \"%s\"", textureLoading.path.c_str());
    return NULL;
  }

  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  bool mipmap = true;

  float aniso = 0.0f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

  if (mipmap) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  } else {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

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
  int channels;
  return SOIL_load_image(path, width, height, &channels, SOIL_LOAD_RGB);
}

void Loader::load_mesh(const char *path, Renderer *gl) {
  printf("Loading mesh %s\n", path);

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    printf("Error: %s\n", importer.GetErrorString());
  }

  std::vector<LoadedMesh> *meshes = new std::vector<LoadedMesh>();

  models[path] = meshes;

  for (unsigned int m=0; m<scene->mNumMaterials; ++m) {
    {
      int texIndex = 0;
      aiString path;

      aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_HEIGHT, texIndex, &path);
      while (texFound == AI_SUCCESS) {
        std::string fullPath = std::string(path.data);
        add_texture(fullPath);
        texIndex++;
        texFound = scene->mMaterials[m]->GetTexture(aiTextureType_HEIGHT, texIndex, &path);
      }
    }

    {
      int texIndex = 0;
      aiString path;

      aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, texIndex, &path);
      while (texFound == AI_SUCCESS) {
        std::string fullPath = std::string(path.data);
        add_texture(fullPath);
        texIndex++;
        texFound = scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, texIndex, &path);
      }
    }

    {
      int texIndex = 0;
      aiString path;

      aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
      while (texFound == AI_SUCCESS) {
        std::string fullPath = std::string(path.data);
        add_texture(fullPath);
        texIndex++;
        texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
      }
    }

    {
      int texIndex = 0;
      aiString path;

      aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_OPACITY, texIndex, &path);
      while (texFound == AI_SUCCESS) {
        std::string fullPath = std::string(path.data);
        add_texture(fullPath);
        texIndex++;
        texFound = scene->mMaterials[m]->GetTexture(aiTextureType_OPACITY, texIndex, &path);
      }
    }
  }


  if (scene->HasMeshes()) {
    for (int i=0; i<scene->mNumMeshes; i++) {
      aiMesh *meshData = scene->mMeshes[i];

      Mesh *mesh = new Mesh();

      glm::vec3 center(0.0, 0.0, 0.0);

      int count = 0;
      for (int l=0; l<meshData->mNumVertices; l++) {
        count += 1;
        mesh->vertices.push_back(meshData->mVertices[l].x);
        mesh->vertices.push_back(meshData->mVertices[l].y);
        mesh->vertices.push_back(meshData->mVertices[l].z);

        center.x += meshData->mVertices[l].x;
        center.y += meshData->mVertices[l].y;
        center.z += meshData->mVertices[l].z;

        mesh->normals.push_back(meshData->mNormals[l].x);
        mesh->normals.push_back(meshData->mNormals[l].y);
        mesh->normals.push_back(meshData->mNormals[l].z);

        mesh->tangents.push_back(meshData->mTangents[l].x);
        mesh->tangents.push_back(meshData->mTangents[l].y);
        mesh->tangents.push_back(meshData->mTangents[l].z);

        if (meshData->mTextureCoords[0]) {
          mesh->uv.push_back(meshData->mTextureCoords[0][l].x);
          mesh->uv.push_back(meshData->mTextureCoords[0][l].y);
        }
      }

      center = center / float(count);
      mesh->relative_center = center;

      float radius = 0.0f;
      for (int l=0; l<meshData->mNumVertices; l++) {
        float distance = glm::distance(mesh->relative_center, glm::vec3(meshData->mVertices[l].x, meshData->mVertices[l].y, meshData->mVertices[l].z));
        if (distance > radius) {
          radius = distance;
        }
      }

      for (int l=0; l<meshData->mNumFaces; l++) {
        aiFace face = meshData->mFaces[l];

        for (int j=0; j<face.mNumIndices; j++) {
          mesh->indices.push_back(face.mIndices[j]);
        }
      }

      std::string texture_name;
      std::string normal_name;
      std::string specular_name;
      std::string alpha_name;

      if (scene->HasMaterials()) {
        const aiMaterial* material = scene->mMaterials[meshData->mMaterialIndex];

        aiString texturePath;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
          texture_name = std::string(texturePath.data);
        }

        if (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS) {
          normal_name = std::string(texturePath.data);
        }

        if (material->GetTextureCount(aiTextureType_SPECULAR) > 0 && material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS) {
          specular_name = std::string(texturePath.data);
        }

        if (material->GetTextureCount(aiTextureType_OPACITY) > 0 && material->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS) {
          alpha_name = std::string(texturePath.data);
        }
      }

      gl->populate_buffers(mesh);

      mesh->bounding_radius = radius;

      LoadedMesh loaded;
      loaded.mesh = mesh;
      loaded.texture_name = texture_name;
      loaded.normal_name = normal_name;
      loaded.specular_name = specular_name;
      loaded.alpha_name = alpha_name;

      meshes->push_back(loaded);
    }
  }
}
