#version 330

uniform sampler2D uSampler;
uniform sampler2D uDepth;

in vec2 pos;

uniform float zNear = 0.1;
uniform float zFar = 10000.0;

uniform vec3 fogColor;
uniform float density;

out vec4 fragColor;

float linearizeDepth(float depth) {
  depth = depth * 2.0 - 1.0;
  return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main() {
  vec2 uv = pos * 0.5 + 0.5;

  float depth = linearizeDepth(texture(uDepth, uv).r);

  vec3 result = texture(uSampler, uv).xyz;

  float fog = clamp(exp2(density * depth * depth * 1.442695), 0.0, 1.0);

  fragColor = vec4(mix(fogColor, result, fog), 1.0);
}
