#include "scene.h"

float get_random_float(float min, float max) {
  return ((float)rand())/((float)RAND_MAX) * (max - min) + min;
}

void Scene::init(Loader *loader, Renderer *gl) {
  std::vector<LoadedMesh> *model = loader->getModel("sponza.obj");

  for (auto it = model->begin(); it != model->end(); it++) {
    Entity *entity = new Entity();

    if (!it->texture_name.empty()) {
      entity->texture = loader->get(it->texture_name.c_str());
    } else {
      entity->texture = loader->get("default.dds");
    }

    if (!it->normal_name.empty()) {
      entity->normal_map = loader->get(it->normal_name.c_str());
    }

    if (!it->specular_name.empty()) {
      entity->specularTexture = loader->get(it->specular_name.c_str());
    } else {
      entity->specularTexture = loader->get("spec.dds");
    }

    if (!it->alpha_name.empty()) {
      entity->alpha_texture = loader->get(it->alpha_name.c_str());
    } else {
      entity->alpha_texture = loader->get("alpha.dds");
    }

    entity->type = kOther;
    entity->scale = glm::vec3(1.0);
    entity->mesh = it->mesh;
    entity->rotation = glm::vec3(0.0, 0.0, 0.0);
    entity->position = glm::vec3(0.0, 0.0, 0.0);
    entities.push_back(entity);
  }

  {
    Light *light = new Light();
    light->type = kAmbient;
    light->color = glm::vec3(0.2);
    lights.push_back(light);
  }

  {
    Light *light = new Light();
    light->type = kPoint;
    light->radius = 4500.0f;
    light->position = glm::vec3(0.195040, 2415.061523, -5.368996);
    light->color = glm::vec3(0.8, 0.8, 0.89);
    light->isCastingShadow = true;
    light->camera.position = light->position;
    light->camera.rotation = glm::vec3(1.569796, 0.010000, 0.000000);
    light->camera.fov = 90.0f;
    light->camera.far = light->radius;
    light->attenuation = 4000;
    light->camera.update_matrix();
    light->frame_buffer.init(4096, 4096, true);
    lights.push_back(light);
  }
}
