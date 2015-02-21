#version 330

in vec2 outUv;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform vec3 lightColor;
uniform vec3 lightDirection;

out vec4 fragColor;

void main() {
  vec3 normalColor = texture(normalTexture, outUv).xyz;
  vec3 diffuseColor = texture(diffuseTexture, outUv).xyz;
  vec3 positionColor = texture(positionTexture, outUv).xyz;

  float directionalLightWeighting = max(dot(normalColor, lightDirection), 0.0);
  vec3 result = diffuseColor * lightColor * directionalLightWeighting;

  fragColor = vec4(result, 1.0);
  // fragColor = diffuseColor;
}
