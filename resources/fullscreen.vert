#version 330

in vec3 position;
in vec2 uv;

out vec2 outUv;

void main() {
  outUv = uv.st;
  gl_Position = vec4(position, 1.0);
}
