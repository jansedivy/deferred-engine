#version 330

in vec3 position;

uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;

void main() {
  gl_Position = uPMatrix * uMVMatrix * vec4(position, 1.0);
}
