#version 330

in vec3 position;
in vec3 normals;
in vec3 tangents;
in vec2 uv;

uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;

out vec4 vPosition;
out vec2 texturePosition;
out mat3 tbnMatrix;

void main() {
  texturePosition = uv;
  vPosition = uMVMatrix * vec4(position, 1.0);

  vec3 n = normalize((uMVMatrix * vec4(normals, 0.0)).xyz);
  vec3 t = normalize((uMVMatrix * vec4(tangents, 0.0)).xyz);

  t = normalize(t - dot(t, n) * n);

  vec3 biTangent = cross(t, n);
  tbnMatrix = mat3(t, biTangent, n);

  gl_Position = uPMatrix * vPosition;
}
