#version 330

uniform sampler2D diffuseTexture;

uniform vec3 lightColor;

out vec4 fragColor;

in vec2 pos;

void main() {
  vec2 uv = pos * 0.5 + 0.5;

  vec3 diffuseColor = texture(diffuseTexture, uv).xyz;

  fragColor = vec4(diffuseColor * lightColor, 1.0);
}
