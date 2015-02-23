#pragma once

#include "shader.h"
#include "file_io.h"
#include "error.h"

#include <map>
#include <string>

class ShaderManager {
  public:
    Shader* get(const char *name);
    void load(const char *name, const char *path);
    void use(const char *name);

    Shader *current;

  private:
    std::map<std::string, Shader*> shaders;
};
