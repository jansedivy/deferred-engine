#pragma once

#include <GL/glew.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include "file_io.h"

#include <iostream>
#include <map>
#include <vector>

class Shader {
  public:
    Shader() {};
    Shader(std::string vertexPath, std::string fragmentPath, std::string vertexSource, std::string fragmentSource);
    void use();
    void disable();

    void setUniform(const char *name, glm::vec3 value);
    void setUniform(const char *name, glm::mat4 value);
    void setUniform(const char *name, glm::mat3 value);
    void setUniform(const char *name, float value);

    void texture(const char *name, GLuint texture, int id);
    void cubemap(const char *name, GLuint texture, int id);

    void addAttribute(const char *name);
    void addUniform(const char *name);

    std::map<std::string, GLuint> attributes;
    std::map<std::string, GLuint> uniforms;

    GLuint program;

    void tryReload();

    time_t lastWriteVertex;
    time_t lastWriteFragment;

  private:
    void checkError(int shader, bool isProgram);
};

