#version 330

in vec2 texturePosition;
in mat3 tbnMatrix;

in vec3 inNormals;

uniform sampler2D uSampler;
uniform sampler2D uNormalMap;
uniform sampler2D uSpecular;

uniform int hasNormalMap;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 specularOut;

vec4 encode(vec3 n) {
  return vec4(n.xyz * 0.5 + 0.5, 0.0);
}

void main() {
  vec3 normalMap;

  if (hasNormalMap == 1) {
    normalMap = normalize(tbnMatrix * normalize(texture(uNormalMap, texturePosition).rgb * 2.0 - 1.0));
  } else {
    normalMap = normalize(inNormals);
  }

  fragColor = vec4(texture(uSampler, texturePosition).xyz, 1.0);
  normalOut = encode(normalMap);
  specularOut = vec4(texture(uSpecular, texturePosition).xyz, 1.0);
}
