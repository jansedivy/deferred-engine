#version 330

in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 texturePosition;

void main() {
  texturePosition = position;
  gl_Position = projection * view * vec4(position, 1.0);
}
