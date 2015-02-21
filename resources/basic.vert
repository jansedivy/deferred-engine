#version 330

in vec3 position;
in vec3 normals;
in vec2 uv;

uniform mat4 uPMatrix;
uniform mat3 uNMatrix;
uniform mat4 uMVMatrix;

out vec3 transformedNormals;
out vec4 vPosition;
out vec2 texturePosition;

void main() {
  texturePosition = uv;
  vPosition = uMVMatrix * vec4(position, 1.0);
  transformedNormals = uNMatrix * normals;
  gl_Position = uPMatrix * vPosition;
}
