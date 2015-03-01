#include "shader_manager.h"

Shader* ShaderManager::get(const char *name) {
  auto it = shaders.find(std::string(name));

  if (it != shaders.end()) {
    return (it->second);
  } else {
    showError("Error undefined shader", "Error undefined shader %s", name);
    exit(1);
  }

  return NULL;
}

void ShaderManager::load(const char *name, const char *path) {
  std::string stringPath = path;

  std::string vertexName = stringPath + ".vert.glsl";
  std::string fragName = stringPath + ".frag.glsl";

  std::string vertexSource = FileIO::loadFile(vertexName.c_str());
  std::string fragmentSource = FileIO::loadFile(fragName.c_str());

  shaders[std::string(name)] = new Shader(std::string(name), vertexSource, fragmentSource);
}

void ShaderManager::use(const char *name) {
  Shader *shader = get(name);
  shader->use();
  current = shader;
}
