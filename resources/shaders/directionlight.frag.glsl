#version 330

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform vec3 lightColor;
uniform vec3 lightDirection;

out vec4 fragColor;

in vec2 pos;

vec3 decode(vec4 enc) {
  return enc.xyz * 2.0 - 1.0;
}

void main() {
  vec2 uv = pos * 0.5 + 0.5;

  vec3 normalColor = decode(texture(normalTexture, uv));
  vec3 diffuseColor = texture(diffuseTexture, uv).xyz;
  vec3 positionColor = texture(positionTexture, uv).xyz;

  float directionalLightWeighting = max(dot(normalColor, lightDirection), 0.0);
  vec3 result = lightColor * directionalLightWeighting;

  fragColor = vec4(diffuseColor, 1.0) * vec4(result, 1.0);
  // fragColor = diffuseColor;
}
