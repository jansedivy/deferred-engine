#version 330

in vec3 position;
in vec3 normals;

uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;

out vec3 normal;

void main() {
  normal = normals * uNMatrix;
  gl_Position = uPMatrix * uMVMatrix * vec4(position, 1.0);
  gl_Position += vec4(normal * 0.001, 0.0);
}
