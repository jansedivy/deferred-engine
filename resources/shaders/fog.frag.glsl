#version 330

uniform sampler2D uSampler;
uniform sampler2D uDepth;

in vec2 pos;

uniform float zNear = 0.1;
uniform float zFar = 10000.0;

out vec4 fragColor;

float linearizeDepth(float depth) {
  depth = depth * 2.0 - 1.0;
  return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main() {
  vec2 uv = pos * 0.5 + 0.5;

  float depth = linearizeDepth(texture(uDepth, uv).r);

  vec3 result = texture(uSampler, uv).zyx;

  vec3 fogColor = vec3(1.0, 0.0, 0.0);
  float density = -3.7;

  float fog = clamp(exp2(density * depth * depth * 1.442695), 0.0, 1.0);
  vec3 color = texture(uSampler, uv).rgb;

  fragColor = vec4(mix(fogColor, color, fog), 1.0);
}
