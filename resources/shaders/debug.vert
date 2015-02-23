#version 330

in vec3 position;

uniform mat4 uPMatrix;

void main() {
  gl_Position = uPMatrix * vec4(position, 1.0);
}
