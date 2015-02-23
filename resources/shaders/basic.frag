#version 330

in vec4 vPosition;
in vec3 transformedNormals;
in vec2 texturePosition;

uniform sampler2D uSampler;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 positionOut;

vec4 encode(vec3 n) {
  return vec4(n.xyz * 0.5 + 0.5, 0.0);
}

void main() {
  vec3 diff = texture(uSampler, texturePosition).xyz;
  vec4 normal = encode(normalize(transformedNormals));

  fragColor = vec4(diff, 1.0);
  normalOut = normal;
  positionOut = vPosition;
}
