#include "shader.h"

Shader::Shader(std::string shaderName, std::string vertexSource, std::string fragmentSource) {
  this->shaderName = shaderName;
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char *vertex = vertexSource.c_str();
  glShaderSource(vertexShader, 1, &vertex, NULL);
  glCompileShader(vertexShader);
  checkError(vertexShader, false);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragment = fragmentSource.c_str();
  glShaderSource(fragmentShader, 1, &fragment, NULL);
  glCompileShader(fragmentShader);
  checkError(fragmentShader, false);

  program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  glLinkProgram(program);
  checkError(program, true);

  int length, size, count;
  char name[256];
  GLenum type = 0;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
  for (int i=0; i<count; i++) {
    glGetActiveAttrib(program, GLuint(i), sizeof(name), &length, &size, &type, name);
    addAttribute(name);
  }

  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
  for (int i=0; i<count; i++) {
    glGetActiveUniform(program, GLuint(i), sizeof(name), &length, &size, &type, name);
    addUniform(name);
  }
}

void Shader::use() {
  glUseProgram(program);

  std::map<std::string, GLuint>::iterator iter;

  for (iter = attributes.begin(); iter != attributes.end(); ++iter) {
    glEnableVertexAttribArray(iter->second);
  }
}

void Shader::disable() {
  std::map<std::string, GLuint>::iterator iter;

  for (iter = attributes.begin(); iter != attributes.end(); ++iter) {
    glDisableVertexAttribArray(iter->second);
  }
}

void Shader::setUniform(const char* name, glm::vec3 value) {
  glUniform3fv(uniforms[name], 1, glm::value_ptr(value));
}

void Shader::setUniform(const char *name, float value) {
  glUniform1f(uniforms[name], value);
}

void Shader::setUniform(const char *name, int value) {
  glUniform1i(uniforms[name], value);
}

void Shader::setUniform(const char *name, glm::mat4 value) {
  glUniformMatrix4fv(uniforms[name], 1, false, glm::value_ptr(value));
}

void Shader::setUniform(const char *name, glm::mat3 value) {
  glUniformMatrix3fv(uniforms[name], 1, false, glm::value_ptr(value));
}

void Shader::texture(const char *name, GLuint texture, int id) {
  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(uniforms[name], id);
}

void Shader::cubemap(const char *name, GLuint texture, int id) {
  glActiveTexture(GL_TEXTURE0 + id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glUniform1i(uniforms[name], id);
}

void Shader::checkError(int shader, bool isProgram) {
  GLint result = 0;
  int logLength;

  if (isProgram) {
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
  } else {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  }

  if (result == false) {
    std::vector<char> errorMessage((logLength > 1) ? logLength : 1);

    if (isProgram) {
      glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);
      glGetProgramInfoLog(shader, logLength, NULL, &errorMessage[0]);
    } else {
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
      glGetShaderInfoLog(shader, logLength, NULL, &errorMessage[0]);
    }

    showError("Shader error", "Shader: %s: %s\n", shaderName.c_str(), &errorMessage[0]);
  }
}

void Shader::addAttribute(const char *name) {
  GLuint id = glGetAttribLocation(program, name);

  if (id == -1) {
    showError("Error undefined attribute", "Undefined attribute \"%s\"", name);
  }

  attributes[name] = id;
}

void Shader::addUniform(const char *name) {
  GLuint id = glGetUniformLocation(program, name);

  if (id == -1) {
    showError("Error undefined uniform", "Undefined uniform \"%s\"", name);
  }

  uniforms[name] = id;
}
