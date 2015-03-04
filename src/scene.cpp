#include "scene.h"

void Scene::init(Loader *loader, Renderer *gl) {
  std::vector<LoadedMesh> *model = loader->getModel("sponza.obj");

  for (auto it = model->begin(); it != model->end(); it++) {
    Entity entity = { 0 };

    if (!it->textureName.empty()) {
      entity.texture = loader->get(it->textureName.c_str());
    } else {
      entity.texture = loader->get("default.dds");
    }

    if (!it->normalName.empty()) {
      entity.normalMap = loader->get(it->normalName.c_str());
    }

    if (!it->specularName.empty()) {
      entity.specularTexture = loader->get(it->specularName.c_str());
    } else {
      entity.specularTexture = loader->get("spec.dds");
    }

    if (!it->alphaName.empty()) {
      entity.alphaTexture = loader->get(it->alphaName.c_str());
    } else {
      entity.alphaTexture = loader->get("alpha.dds");
    }

    entity.type = kOther;
    entity.scale = glm::vec3(1.0, 1.0, 1.0);
    entity.mesh = it->mesh;
    entity.rotation = glm::vec3(0.0, 0.0, 0.0);
    entity.position = glm::vec3(-2000.0, 0.0, -1000.0);
    entities.push_back(entity);
  }

  Light light;
  light.type = kAmbient;
  light.color = glm::vec3(0.1, 0.1, 0.1);
  lights.push_back(light);
}
