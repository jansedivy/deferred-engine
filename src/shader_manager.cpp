#include "shader_manager.h"

Shader* ShaderManager::get(const char *name) {
  auto it = shaders.find(std::string(name));

  if (it != shaders.end()) {
    return (it->second);
  } else {
    showError("Error undefined shader", "Error undefined shader %s"); // name
    exit(1);
  }

  return NULL;
}

void ShaderManager::load(const char *name, const char *path) {
  std::string stringName = name;

  std::string vertexName = stringName + ".vert";
  std::string fragName = stringName + ".frag";

  std::string vertexSource = FileIO::loadFile(vertexName.c_str());
  std::string fragmentSource = FileIO::loadFile(fragName.c_str());

  shaders[stringName] = new Shader(vertexName, fragName, vertexSource, fragmentSource);
}
