#version 330

uniform sampler2D uSampler;

out vec4 fragColor;

in vec2 pos;

void main() {
  vec2 uv = pos * 0.5 + 0.5;

  fragColor = texture(uSampler, uv);
}
