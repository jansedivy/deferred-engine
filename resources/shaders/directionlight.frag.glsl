#version 330

in vec2 outUv;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform vec3 lightColor;
uniform vec3 lightDirection;

out vec4 fragColor;

vec3 decode(vec4 enc) {
  return enc.xyz * 2.0 - 1.0;
}

void main() {
  vec3 normalColor = decode(texture(normalTexture, outUv));
  vec3 diffuseColor = texture(diffuseTexture, outUv).xyz;
  vec3 positionColor = texture(positionTexture, outUv).xyz;

  float directionalLightWeighting = max(dot(normalColor, lightDirection), 0.0);
  vec3 result = lightColor * directionalLightWeighting;

  fragColor = vec4(diffuseColor, 1.0) * vec4(result, 1.0);
  // fragColor = diffuseColor;
}
