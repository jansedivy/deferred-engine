#version 330

in vec3 position;
in vec3 normals;
in vec3 tangents;
in vec2 uv;

uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;

uniform int hasNormalMap;

out vec4 vPosition;
out vec2 texturePosition;
out mat3 tbnMatrix;
out vec3 inNormals;

void main() {
  texturePosition = uv.st * vec2(1.0, -1.0);
  vPosition = uMVMatrix * vec4(position, 1.0);

  if (hasNormalMap == 1) {
    vec3 n = normalize(uNMatrix * normals);
    vec3 t = normalize(uNMatrix * tangents);

    t = normalize(t - dot(t, n) * n);

    vec3 biTangent = cross(t, n);
    tbnMatrix = mat3(t, biTangent, n);
  } else {
    inNormals = uNMatrix * normals;
  }

  gl_Position = uPMatrix * vPosition;
}
