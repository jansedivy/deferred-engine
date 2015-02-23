#pragma once

#include "shader.h"
#include <map>
#include "file_io.h"
#include <string>

class ShaderManager {
  public:
    Shader* get(const char *name);
    void load(const char *name, const char *path);

  private:
    std::map<std::string, Shader*> shaders;
};
